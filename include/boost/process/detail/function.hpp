//
// Created by dudilikvornik on 3/27/19.
//

#ifndef BOOST_PROCESS_DETAIL_FUNCTION_H
#define BOOST_PROCESS_DETAIL_FUNCTION_H

#include <functional>
#include <type_traits>
#include <boost/type_traits/integral_constant.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/process/detail/traits/decl.hpp>
#include <boost/process/detail/traits/function.hpp>

#if defined( BOOST_WINDOWS_API )
#elif defined( BOOST_POSIX_API )
#include <boost/process/detail/posix/function.hpp>
#endif

namespace boost { namespace process { namespace detail {

template<typename... Args>
struct f_arg_setter_
{
private:
    typedef fusion::vector<Args...> vector_type;
    vector_type m_args;
    
public:
    typedef typename fusion::result_of::begin<vector_type>::type begin_iterator;
    typedef typename fusion::result_of::end<vector_type>::type end_iterator;
    
    template<typename... _Args>
    f_arg_setter_(_Args&&... args): m_args(std::forward<_Args>(args)...){}
    begin_iterator begin() { return fusion::begin(m_args); }
    end_iterator end() { return fusion::end(m_args); }
};

template<typename T1, typename T2>
using equal_t = fusion::result_of::equal_to<T1, T2>;

template< typename FIT_1, typename FIEndT_1, typename FIT_2 >
void move_assign_fusion_container( FIT_1 it_1, FIEndT_1 end_1, FIT_2 it_2, boost::true_type){}
template< typename FIT_1, typename FIEndT_1, typename FIT_2 >
void move_assign_fusion_container( FIT_1 it_1, FIEndT_1 end_1, FIT_2 it_2, boost::false_type)
{
    *it_1 = std::move(*it_2);
    move_assign_fusion_container(fusion::next(it_1), end_1, fusion::next(it_2),
            equal_t<typename fusion::result_of::next<FIT_1>::type, FIEndT_1>());
}

template<typename... Args>
struct function_builder
{
private:
    typedef fusion::vector<Args...> vector_type;
    typedef std::function<void(Args...)> function_type;
    vector_type m_args;
    function_type m_func;
    
public:
    typedef api::function_init<Args...> result_type;
    template<typename Callable, typename = typename std::enable_if<
            std::is_assignable<function_type, Callable>::value>::type>
    void operator()(Callable func){ m_func = func; }
    void operator()(f_arg_setter_<Args...>& args)
    {
        typedef typename fusion::result_of::begin<vector_type>::type FIBeginT;
        typedef typename fusion::result_of::end<vector_type>::type FIEndT;
        move_assign_fusion_container(fusion::begin(m_args),
                fusion::end(m_args), args.begin(),
                equal_t<FIBeginT, FIEndT>());
    }
    
    result_type get_initializer()
    {
        return result_type(m_func, std::move(m_args));
    }
};

template<typename... Args>
struct initializer_builder<function_arg_tag<Args...>>
{
    typedef function_builder<Args...> type;
};
}}}
#endif //BOOST_FUNCTION_H
