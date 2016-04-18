// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_DETAIL_POSIX_HANDLER_HPP_
#define BOOST_PROCESS_DETAIL_POSIX_HANDLER_HPP_

#include <boost/process/detail/handler_base.hpp>

namespace boost { namespace process { namespace detail { namespace posix {

//does not extend anything.
struct handler_base_ext : handler_base 
{
	template<typename Executor>
	void on_fork_error  (Executor &, const std::error_code&) const {}

	template<typename Executor>
	void on_exec_setup  (Executor &) const {}

	template<typename Executor>
	void on_exec_error  (Executor &, const std::error_code&) const {}

};


template <class Handler>
struct on_fork_error_ : handler_base_ext
{
    explicit on_fork_error_(Handler handler) : handler_(handler) {}

    template <class Executor>
    void on_fork_error(Executor &e) const
    {
        handler_(e);
    }
private:
    Handler handler_;
};


template <class Handler>
struct on_exec_setup_ : handler_base_ext
{
    explicit on_exec_setup_(Handler handler) : handler_(handler) {}

    template <class Executor>
    void on_exec_setup(Executor &e) const
    {
        handler_(e);
    }
private:
    Handler handler_;
};

template <class Handler>
struct on_exec_error_ : handler_base_ext
{
    explicit on_exec_error_(Handler handler) : handler_(handler) {}

    template <class Executor>
    void on_exec_error(Executor &e) const
    {
        handler_(e);
    }
private:
    Handler handler_;
};



constexpr boost::process::detail::make_handler_t<on_fork_error_  >   on_fork_error;
constexpr boost::process::detail::make_handler_t<on_exec_setup_  >   on_exec_setup;
constexpr boost::process::detail::make_handler_t<on_exec_error_	 >   on_exec_error;

}}}}



#endif /* BOOST_PROCESS_DETAIL_POSIX_HANDLER_HPP_ */
