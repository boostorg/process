//
// Created by dudilikvornik on 3/27/19.
//

#ifndef BOOST_PROCESS_DETAIL_POSIX_FUNCTION_INVOKER_H
#define BOOST_PROCESS_DETAIL_POSIX_FUNCTION_INVOKER_H

#include <utility>
#include <memory>
#include <boost/fusion/container/vector.hpp>
#include <boost/mp11/integer_sequence.hpp>

namespace boost{ namespace process{ namespace detail{ namespace posix {

struct function_invoker_base
{
public:
    typedef std::shared_ptr<function_invoker_base> function_invoker_ptr;
    virtual ~function_invoker_base() noexcept = default;
    virtual void invoke() = 0;
};

template<typename... Args>
struct function_invoker : public function_invoker_base
{
private:
    typedef fusion::vector<Args...> vector_type;
    typedef std::function<void(Args...)> function_type;
    function_type m_func;
    vector_type m_args;

public:
    ~function_invoker() noexcept override = default;
    function_invoker(function_type func, vector_type&& args)
        :m_func(func), m_args(std::move(args))
    {
    }
    void invoke() override
    {
        invoke_impl(mp11::make_index_sequence<sizeof...(Args)>());
    }
    
private:
    template<std::size_t... Index>
    void invoke_impl(mp11::index_sequence<Index...>)
    {
        m_func(fusion::at_c<Index>(m_args)...);
    }
};

}}}}
#endif //BOOST_FUNCTION_INVOKER_H
