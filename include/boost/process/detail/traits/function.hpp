//
// Created by dudilikvornik on 3/27/19.
//

#ifndef BOOST_PROCESS_DETAIL_TRAITS_FUNCTION_H
#define BOOST_PROCESS_DETAIL_TRAITS_FUNCTION_H

#include <boost/process/detail/traits/decl.hpp>
#include <boost/mp11/integer_sequence.hpp>
#include <boost/fusion/container/vector.hpp>

namespace boost { namespace process { namespace detail {

template<typename... Args> struct f_arg_setter_;

template<typename... Args>
struct function_arg_tag {};

template<typename... Args>
struct initializer_tag<f_arg_setter_<Args...>, void> { typedef function_arg_tag<Args...> type; };

struct is_callable
{
    template<typename, typename...>
    static constexpr bool test(...){return false;}
    
    template<typename T, typename... Args, typename = decltype(std::declval<T>().operator()(std::declval<Args>()...))>
    static constexpr bool test(int){return true;}
};

template<typename... Args>
struct function_params{};

template<typename T, typename... Args>
struct get_callable_params{ typedef void type; };
template<typename T, typename... Args>
struct get_callable_params<void(T::*)(Args...)const>
{
    typedef function_params<Args...> type;
};

template<size_t Index>
struct argument_
{
public:
    template<typename T>
    constexpr operator T() const;
};

template<typename T, typename Enabled, typename... Args>
struct callable_arguments_types_detector_impl
{
    typedef void type;
    static const bool value = false;
};


template<typename T>
struct callable_arguments_types_detector_impl<T,
        typename std::enable_if<std::is_member_function_pointer<decltype(&T::operator())>::value>::type>
{
    typedef typename std::conditional<is_callable::test<T>(0),
    typename get_callable_params<decltype(&T::operator())>::type,
    void>::type type;
    
    static const bool value = is_callable::test<T>(0);
};


template<typename T, typename Arg0, typename... Args>
struct callable_arguments_types_detector_impl<T,
        typename std::enable_if<std::is_member_function_pointer<decltype(&T::operator())>::value>::type,
        Arg0, Args...>
{
    typedef typename std::conditional<is_callable::test<T, Arg0, Args...>(0),
    typename get_callable_params<decltype(&T::operator())>::type,
    typename callable_arguments_types_detector_impl<T, void, Args...>::type>::type type;

    static const bool value = is_callable::test<T, Arg0, Args...>(0) || callable_arguments_types_detector_impl<T, void, Args...>::value;
};

template<typename T, std::size_t... Index>
struct callable_argument_types_detector
{
    typedef typename callable_arguments_types_detector_impl<typename std::decay<T>::type, void, argument_<Index>...>::type type;
    static const bool value = callable_arguments_types_detector_impl<typename std::decay<T>::type, void, argument_<Index>...>::value;
};

template<typename T, typename X>
struct callable_argument_types_impl{};

template<typename T, std::size_t... Index>
struct callable_argument_types_impl<T, mp11::index_sequence<Index...>&&>
{
    typedef typename callable_argument_types_detector<T, Index...>::type type;
    static const bool value = callable_argument_types_detector<T, Index...>::value;
};

template<typename T, std::size_t Argument_Count>
struct callable_argument_types :
        public callable_argument_types_impl<T, decltype(std::declval<mp11::make_index_sequence<Argument_Count>>())>
{
};

template<typename... Args>
struct initializer_tag<function_params<Args...>, void> { typedef function_arg_tag<typename std::decay<Args>::type...> type;};

template<typename Callable>
using callable_t = callable_argument_types<Callable, 10>;

template<typename Callable>
struct initializer_tag<Callable, typename std::enable_if<callable_t<Callable>::value>::type>
{
    typedef typename initializer_tag<typename callable_t<Callable>::type>::type type;
};


}}}
#endif //BOOST_FUNCTION_H
