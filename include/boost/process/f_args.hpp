//
// Created by dudilikvornik on 3/27/19.
//
#ifndef BOOST_PROCESS_F_ARGS_H
#define BOOST_PROCESS_F_ARGS_H

#include <type_traits>
#include <boost/process/detail/function.hpp>

namespace boost { namespace process { namespace detail {

struct f_args_
{
public:
    template<typename T>
    using decay_t = typename std::decay<T>::type;

    template<typename... Args>
    f_arg_setter_<decay_t<Args>...> operator()(Args&&... arg) const
    {
        return f_arg_setter_<decay_t<Args>...>(std::forward<Args>(arg)...);
    }
};
}
        
constexpr boost::process::detail::f_args_ f_args{};
}}

#endif //BOOST_PROCESS_F_ARGS_H
