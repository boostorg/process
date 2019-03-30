//
// Created by dudilikvornik on 3/27/19.
//
#ifndef BOOST_PROCESS_DETAIL_POSIX_FUNCTION_HPP
#define BOOST_PROCESS_DETAIL_POSIX_FUNCTION_HPP

#include <functional>
#include <boost/process/detail/posix/handler.hpp>
#include <boost/process/detail/posix/function_invoker.hpp>
#include <boost/fusion/container/vector.hpp>

namespace boost{ namespace process{ namespace detail{ namespace posix {

template<typename... Args>
struct function_init : boost::process::detail::api::handler_base_ext
{
private:
    typedef fusion::vector<Args...> vector_type;
    typedef std::function<void(Args...)> function_type;
    typedef function_invoker_base::function_invoker_ptr function_invoker_ptr;
    function_invoker_ptr m_function_invoker;
    
public:
    function_init(function_type func, vector_type&& args)
        :m_function_invoker(new function_invoker<Args...>(func, std::move(args)))
        {}
    
    template<class Executor>
    void on_setup(Executor &exec)
    {
        exec.function_invoker = std::move(m_function_invoker);
        exec.function_invocation = true;
    }
};
}}}}
#endif //BOOST_PROCESS_DETAIL_POSIX_FUNCTION_HPP
