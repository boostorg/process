// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_PROCESS_V2_GROUP_IMPL_HPP
#define BOOST_PROCESS_V2_GROUP_IMPL_HPP

#include <boost/process/v2/detail/config.hpp>

#if defined(BOOST_PROCESS_V2_STANDALONE)
#include <asio/any_io_executor.hpp>
#include <asio/append.hpp>
#include <asio/compose.hpp>
#include <asio/dispatch.hpp>
#include <asio/post.hpp>
#include <asio/windows/basic_object_handle.hpp>
#else
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/append.hpp>
#include <boost/asio/compose.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/windows/basic_object_handle.hpp>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE

struct single_process_exit;

namespace detail
{

BOOST_PROCESS_V2_DECL bool job_object_is_empty(HANDLE job_object, error_code & ec);

template<typename Allocator>
BOOST_PROCESS_V2_DECL std::pair<DWORD, DWORD> job_object_something_exited(HANDLE job_object, std::vector<int> & store, Allocator alloc_, error_code & ec)
{
    // let's start with a 
    typename std::aligned_storage<sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST) * 16, alignof(JOBOBJECT_BASIC_PROCESS_ID_LIST)>::type storage;
    JOBOBJECT_BASIC_PROCESS_ID_LIST * id_list = static_cast<JOBOBJECT_BASIC_PROCESS_ID_LIST*>(storage);

    if (!QueryInformationJobObject(job_object,
            JobObjectBasicProcessIdList,
            id_list, sizeof(storage), nullptr))
        ec = detail::get_last_error();
    
    if (ec)
        return {};

    using allocator_type = std::allocator_traits<Allocator>::rebind_alloc<JOBOBJECT_BASIC_PROCESS_ID_LIST>;
    allocator_type alloc{alloc_};

    std::size_t sz = (std::numeric_limits<std::size_t>::max)();
    // dit not fit in the buffer, alloc a buffer
    if (id_list.NumberOfAssignedProcesses != id_list.NumberOfProcessIdsInList)
    {
        // required size:
        const additional_size = id_list.NumberOfAssignedProcesses - 1;
        sz = (additional_size /  (sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST) / sizeof(ULONG_PTR))) + 1;
        id_list = std::allocator_traits<allocator_type>::allocate(alloc_, sz);

        if (!QueryInformationJobObject(job_object,
                JobObjectBasicProcessIdList,
                id_list, sz, nullptr))
        {
            ec = detail::get_last_error();
            goto complete;
        }

    }
    std::pair<DWORD, DWORD> result;

    auto * begin = id_list->ProcessIdList,
         * end   = id_list->ProcessIdList + id_list->NumberOfProcessIdsInList;

    for (auto itr = store.begin(); itr != store.end(); itr++)
    {
        // cross check if it's in the job object
        auto it = std::find(begin, end, *itr);
        if (it == end) // missing a job
        {
            result.first = *itr;
           // ::GetExitCodeProcess(); // this can't be done based on PID, i need the f'in handle.
        }
    }


  complete:
    if (sz != (std::numeric_limits<std::size_t>::max)())
        std::allocator_traits<allocator_type>::deallocate(alloc_, id_list, sz);

    return result;
}

#if !defined(BOOST_PROCESS_V2_HEADER_ONLY)
//extern template 
#endif

struct basic_group_impl_base
{
    using native_handle_type = HANDLE;

    struct job_object_deleter
    {
        bool terminate_on_delete = true;
        void operator()(HANDLE h) 
        {
          if (h != nullptr && terminate_on_delete)
            ::TerminateJobObject(h, 255u);
          if (h != nullptr && h != INVALID_HANDLE_VALUE) 
            ::CloseHandle(h); 
        }
    };
    BOOST_PROCESS_V2_DECL void add(DWORD pid, HANDLE handle, error_code & ec);
    BOOST_PROCESS_V2_DECL bool contains(DWORD  pid, error_code & ec);
    BOOST_PROCESS_V2_DECL void wait_one(DWORD &pid, int &exit_code, error_code & ec);
    BOOST_PROCESS_V2_DECL void wait_all(error_code & ec);
    BOOST_PROCESS_V2_DECL void interrupt(error_code & ec);
    BOOST_PROCESS_V2_DECL void request_exit(error_code & ec);
    BOOST_PROCESS_V2_DECL void terminate(error_code & ec);
    bool is_open() const
    {
        return job_object_.get() != nullptr;
    }

    struct initializer_t 
    {
        basic_group_impl_base * self;
        error_code & success_ec;

        error_code on_setup(windows::default_launcher & launcher,
                      const filesystem::path &, 
                      const std::wstring &) const
        {
            launcher.creation_flags |= CREATE_SUSPENDED;
            return error_code {};
        };


        void on_success(windows::default_launcher & launcher,
                        const filesystem::path &, 
                        const std::wstring &) const
        {
            if (!::AssignProcessToJobObject(self->job_object_.get(), launcher.process_information.hProcess))
                success_ec = detail::get_last_error();

            if (!success_ec && 
                ::ResumeThread(launcher.process_information.hThread) == static_cast<DWORD>(-1))
                success_ec = detail::get_last_error();
        };
    };
    initializer_t get_initializer(error_code & ec)
    {
        return initializer_t{this, ec};
    }
    basic_group_impl_base(BOOST_PROCESS_V2_ASIO_NAMESPACE::execution_context & ctx) 
    {
    }
    ~basic_group_impl_base()
    {       
    }
    void detach() {job_object_.get_deleter().terminate_on_delete = false;}
  protected:
    std::unique_ptr<void, job_object_deleter> job_object_{CreateJobObject(nullptr, nullptr)};
};

template<typename Executor = BOOST_PROCESS_V2_ASIO_NAMESPACE::any_io_executor>
struct basic_group_impl :  basic_group_impl_base
{

    // Get the native group handle
    native_handle_type native_handle() {return port_.native_handle();}

    basic_group_impl(Executor exec)
        : basic_group_impl_base(BOOST_PROCESS_V2_ASIO_NAMESPACE::query(exec, 
                                BOOST_PROCESS_V2_ASIO_NAMESPACE::execution::context)),  
          port_(exec, ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1))
    {
        error_code ec;
            JOBOBJECT_ASSOCIATE_COMPLETION_PORT port{job_object_.get(), port_.native_handle()};

    if (!::SetInformationJobObject(
            job_object_.get(), 
            JobObjectAssociateCompletionPortInformation,
            &port, sizeof(port)))
        ec = v2::detail::get_last_error();
    }


    template<BOOST_PROCESS_V2_COMPLETION_TOKEN_FOR(void(error_code, single_process_exit))
             WaitHandler BOOST_PROCESS_V2_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
    BOOST_PROCESS_V2_INITFN_AUTO_RESULT_TYPE(WaitHandler, void (error_code, single_process_exit))
    async_wait_one(
        WaitHandler &&handler BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type))
    {
        printf("FOOBAR %d %p\n", port_.native_handle(), this);

        return BOOST_PROCESS_V2_ASIO_NAMESPACE::async_compose<
                    WaitHandler, 
                    void (error_code, single_process_exit)>(
                        wait_one_op{this},
                        handler, port_);
    }

    template<BOOST_PROCESS_V2_COMPLETION_TOKEN_FOR(void(error_code))
             WaitHandler BOOST_PROCESS_V2_DEFAULT_COMPLETION_TOKEN_TYPE(executor_type)>
             BOOST_PROCESS_V2_INITFN_AUTO_RESULT_TYPE(WaitHandler, void (error_code))
    async_wait_all(
        WaitHandler &&handler BOOST_ASIO_DEFAULT_COMPLETION_TOKEN(executor_type))
    {
        return BOOST_PROCESS_V2_ASIO_NAMESPACE::async_compose<
                    WaitHandler, 
                    void (error_code)>(
                        wait_all_op{this},
                        handler, port_);
    }

    using executor_type = Executor;
    executor_type get_executor() {return port_.get_executor();}

    void wait_all(error_code &ec)
    {
        if (job_object_is_empty(job_object_.get(), ec))
            return ;

        DWORD completion_code;
        ULONG_PTR completion_key;
        LPOVERLAPPED overlapped;

        int res;
        while (!!(res = GetQueuedCompletionStatus(
            port_.native_handle(),
            &completion_code,
            &completion_key,
            &overlapped,
            INFINITE)))
        {
            if (completion_code == JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO)
                return;
        }

        if (!res)
            ec = detail::get_last_error();
    }

    void wait_one(DWORD & pid, int & exit_code, error_code &ec)
    {
        if (job_object_is_empty(job_object_.get(), ec))
        {
            ec = BOOST_PROCESS_V2_ASIO_NAMESPACE::error::broken_pipe;
            return ;
        }
        DWORD completion_code;
        ULONG_PTR completion_key;
        LPOVERLAPPED overlapped;
        
        int res;
        while (!!(res = GetQueuedCompletionStatus(
            port_.native_handle(),
            &completion_code,
            &completion_key,
            &overlapped,
            INFINITE)))
        {
            if (completion_code == JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO)
            {
                ec = BOOST_PROCESS_V2_ASIO_NAMESPACE::error::broken_pipe;
                return;
            }
            else if (completion_code == JOB_OBJECT_MSG_EXIT_PROCESS
                  || completion_code == JOB_OBJECT_MSG_ABNORMAL_EXIT_PROCESS)
            {
                pid = *reinterpret_cast<DWORD*>(&overlapped);
                const auto p = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
                
                if (p == INVALID_HANDLE_VALUE || p == nullptr)
                {
                    ec = detail::get_last_error();
                    break;
                }
                
                DWORD code;
                res = ::GetExitCodeProcess(p, &code);
                exit_code = static_cast<int>(code);
                break;
            }
            else if (job_object_is_empty(job_object_.get(), ec))
                break;
        }

        if (!res)
            ec = detail::get_last_error();
    }

  private:
    BOOST_PROCESS_V2_ASIO_NAMESPACE::windows::basic_object_handle<Executor> port_;

    struct wait_one_op
    {
        basic_group_impl * this_;

        template<typename Self>
        void operator()(Self && self)
        {
            printf("FOOBAR %d %p\n", __LINE__, this_);
            BOOST_PROCESS_V2_ASIO_NAMESPACE::dispatch(this_->port_.get_executor(), 
                BOOST_PROCESS_V2_ASIO_NAMESPACE::append(std::move(self), error_code{}));
        }

        template<typename Self>
        void operator()(Self && self, error_code ec)
        {
            using namespace BOOST_PROCESS_V2_ASIO_NAMESPACE;
            single_process_exit res;
            
            /*// we need to install our handler first, THEN check, THEN 
            using allocator_type = associated_allocator_t<typename std::decay<Handler::type>;
            allocator_type allocator = get_associated_allocator(handler);
            */
            printf("FOOBAR %d %p\n", __LINE__, this_);
            if (ec)
                return self.complete(ec, res);
            auto exec = self.get_executor();
            printf("FOOBAR %d %p\n", __LINE__, this_);
            if (job_object_is_empty(this_->job_object_.get(), ec))
                return self.complete(BOOST_PROCESS_V2_ASIO_NAMESPACE::error::broken_pipe, res);
            printf("FOOBAR %d %p\n", __LINE__, this_);

            //check  if done
            if (this->poll_one(ec, res))
                return self.complete(ec, res);
            else
                this_->port_.async_wait(std::move(self));
            printf("FOOBAR %d %p\n", __LINE__, this_);
        }

        bool poll_one(error_code & ec, single_process_exit & se)
        {
            DWORD completion_code;
            ULONG_PTR completion_key;
            LPOVERLAPPED overlapped;
            auto res = GetQueuedCompletionStatus(
                this_->port_.native_handle(),
                &completion_code,
                &completion_key,
                &overlapped,
                0u);
            
            if (!res && ::GetLastError() == ERROR_ABANDONED_WAIT_0)
                return false;

            if (completion_code == JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO)
            {
                ec = BOOST_PROCESS_V2_ASIO_NAMESPACE::error::broken_pipe;
                return true;
            }
            else if (completion_code == JOB_OBJECT_MSG_EXIT_PROCESS
                  || completion_code == JOB_OBJECT_MSG_ABNORMAL_EXIT_PROCESS)
            {
                se.pid = *reinterpret_cast<DWORD*>(&overlapped);
                const auto p = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, se.pid);
                
                if (p == INVALID_HANDLE_VALUE || p == nullptr)
                {
                    ec = detail::get_last_error();
                    return true;
                }
                
                DWORD code;
                res = ::GetExitCodeProcess(p, &code);
                se.exit_code = static_cast<int>(code);
                return true;
            }
            else if (job_object_is_empty(this_->job_object_.get(), ec))
            {
                ec = BOOST_PROCESS_V2_ASIO_NAMESPACE::error::broken_pipe;
                return true;
            }
            return false;
        }
    };

    struct wait_all_op
    {
        basic_group_impl * this_;
        template<typename Self>
        void operator()(Self && self)
        {
            /*
            using namespace BOOST_PROCESS_V2_ASIO_NAMESPACE;
            using allocator_type = associated_allocator_t<typename std::decay<Handler::type>;
            allocator_type allocator = get_associated_allocator(handler);
            */
        }
    };


};

#if !defined(BOOST_PROCESS_V2_HEADER_ONLY)
extern template struct basic_group_impl<BOOST_PROCESS_V2_ASIO_NAMESPACE::any_io_executor>;
#endif

}

BOOST_PROCESS_V2_END_NAMESPACE

#if defined(BOOST_PROCESS_V2_HEADER_ONLY)

#include <boost/process/v2/detail/impl/group_impl_windows.ipp>

#endif




#endif //BOOST_PROCESS_V2_GROUP_IMPL_HPP
