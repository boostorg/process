// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_IO_HPP_
#define BOOST_PROCESS_IO_HPP_

#include <iosfwd>
#include <cstdio>
#include <functional>
#include <utility>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/process/config.hpp>
#include <boost/process/pipe.hpp>
#include <boost/asio/buffer.hpp>

#if defined (BOOST_PROCESS_USE_FUTURE)
#include <future>
#endif

#if defined(BOOST_POSIX_API)
#include <boost/process/posix/close_in.hpp>
#include <boost/process/posix/close_out.hpp>
#include <boost/process/posix/null_in.hpp>
#include <boost/process/posix/null_out.hpp>
#include <boost/process/posix/file_in.hpp>
#include <boost/process/posix/file_out.hpp>
#include <boost/process/posix/pipe_in.hpp>
#include <boost/process/posix/pipe_out.hpp>
#include <boost/process/posix/async_in.hpp>
#include <boost/process/posix/async_out.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/windows/close_in.hpp>
#include <boost/process/windows/close_out.hpp>
#include <boost/process/windows/null_in.hpp>
#include <boost/process/windows/null_out.hpp>
#include <boost/process/windows/file_in.hpp>
#include <boost/process/windows/file_out.hpp>
#include <boost/process/windows/pipe_in.hpp>
#include <boost/process/windows/pipe_out.hpp>
#include <boost/process/windows/async_in.hpp>
#include <boost/process/windows/async_out.hpp>
#endif


namespace boost { namespace process { namespace detail {

struct null_t  {constexpr null_t() {}};
struct close_t;

struct std_in_
{
    constexpr std_in_() {}

    api::close_in close() const {return api::close_in(); }
    api::close_in operator=(const close_t &) const {return api::close_in();}
    api::close_in operator<(const close_t &) const {return api::close_in();}

    api::null_in null() const {return api::null_in();}
    api::null_in operator=(const null_t &) const {return api::null_in();}
    api::null_in operator<(const null_t &) const {return api::null_in();}

    api::file_in operator=(const boost::filesystem::path &p) const {return api::file_in(p);}
    api::file_in operator=(const std::string &p)             const {return api::file_in(p);}
    api::file_in operator=(const char*p)                     const {return api::file_in(p);}

    api::file_in operator<(const boost::filesystem::path &p) const {return api::file_in(p);}
    api::file_in operator<(const std::string &p)             const {return api::file_in(p);}
    api::file_in operator<(const char*p)                     const {return api::file_in(p);}

    api::pipe_in operator=(FILE * f)                                          const {return api::pipe_in(f);}
    api::pipe_in operator=(const boost::iostreams::file_descriptor_source &f) const {return api::pipe_in(f);}
    api::pipe_in operator=(const pipe & p)                                    const {return api::pipe_in(p);}

    api::pipe_in operator<(FILE * f)                                          const {return api::pipe_in(f);}
    api::pipe_in operator<(const boost::iostreams::file_descriptor_source &f) const {return api::pipe_in(f);}
    api::pipe_in operator<(const pipe & p)                                    const {return api::pipe_in(p);}

    api::async_in_buffer<const asio::mutable_buffer> operator=(const asio::mutable_buffer & buf) const {return buf;}
    api::async_in_buffer<const asio::const_buffer  > operator=(const asio::const_buffer   & buf) const {return buf;}
    api::async_in_buffer<asio::streambuf     >       operator=(asio::streambuf            & buf) const {return buf;}

    api::async_in_buffer<const asio::mutable_buffer> operator<(const asio::mutable_buffer & buf) const {return buf;}
    api::async_in_buffer<const asio::const_buffer  > operator<(const asio::const_buffer   & buf) const {return buf;}
    api::async_in_buffer<asio::streambuf     >       operator<(asio::streambuf            & buf) const {return buf;}

};

//-1 == empty.
//1 == stdout
//2 == stderr
template<int p1, int p2 = -1>
struct std_out_
{
    constexpr std_out_() {}

    api::close_out<p1,p2> close() const {return api::close_out<p1,p2>(); }
    api::close_out<p1,p2> operator=(const close_t &) const {return api::close_out<p1,p2>();}
    api::close_out<p1,p2> operator>(const close_t &) const {return api::close_out<p1,p2>();}

    api::null_out<p1,p2> null() const {return api::null_out<p1,p2>();}
    api::null_out<p1,p2> operator=(const null_t &) const {return api::null_out<p1,p2>();}
    api::null_out<p1,p2> operator>(const null_t &) const {return api::null_out<p1,p2>();}

    api::file_out<p1,p2> operator=(const boost::filesystem::path &p) const {return api::file_out<p1,p2>(p);}
    api::file_out<p1,p2> operator=(const std::string &p)             const {return api::file_out<p1,p2>(p);}
    api::file_out<p1,p2> operator=(const char*p)                     const {return api::file_out<p1,p2>(p);}

    api::file_out<p1,p2> operator>(const boost::filesystem::path &p) const {return api::file_out<p1,p2>(p);}
    api::file_out<p1,p2> operator>(const std::string &p)             const {return api::file_out<p1,p2>(p);}
    api::file_out<p1,p2> operator>(const char*p)                     const {return api::file_out<p1,p2>(p);}

    api::pipe_out<p1,p2> operator=(FILE * f)                                        const {return api::pipe_out<p1,p2>(f);}
    api::pipe_out<p1,p2> operator=(const boost::iostreams::file_descriptor_sink &f) const {return api::pipe_out<p1,p2>(f);}
    api::pipe_out<p1,p2> operator=(const pipe & p)                                  const {return api::pipe_out<p1,p2>(p);}

    api::pipe_out<p1,p2> operator>(FILE * f)                                        const {return api::pipe_out<p1,p2>(f);}
    api::pipe_out<p1,p2> operator>(const boost::iostreams::file_descriptor_sink &f) const {return api::pipe_out<p1,p2>(f);}
    api::pipe_out<p1,p2> operator>(const pipe & p)                                  const {return api::pipe_out<p1,p2>(p);}

    api::async_out_buffer<p1, p2, asio::mutable_buffer> operator=(asio::mutable_buffer & buf) const {return buf;}
    api::async_out_buffer<p1, p2, asio::streambuf>      operator=(asio::streambuf & os)       const {return os ;}

    api::async_out_buffer<p1, p2, asio::mutable_buffer> operator>(asio::mutable_buffer & buf) const {return buf;}
    api::async_out_buffer<p1, p2, asio::streambuf>      operator>(asio::streambuf & os)       const {return os ;}

#if defined (BOOST_PROCESS_USE_FUTURE)
    api::async_out_future<p1,p2, std::string>       operator=(std::future<std::string> & fut) const;
    api::async_out_future<p1,p2, std::string>       operator>(std::future<std::string> & fut) const;
    api::async_out_future<p1,p2, std::vector<char>> operator=(std::future<std::vector<char>> & fut) const;
    api::async_out_future<p1,p2, std::vector<char>> operator>(std::future<std::vector<char>> & fut) const;
#endif

    template<int pin, typename = std::enable_if_t<
            (((p1 == 1) && (pin == 2)) ||
             ((p1 == 2) && (pin == 1)))
             && (p2 == -1)>>
    constexpr std_out_<1, 2> operator& (const std_out_<pin> &lhs) const
    {
        return std_out_<1, 2> ();
    }

};

struct close_t
{
    constexpr close_t() {}
    template<int T, int U>
    api::close_out<T,U> operator()(std_out_<T,U>) {return api::close_out<T,U>();}
};

constexpr static close_t close;
constexpr static null_t  null;
constexpr static std_in_ std_in;
constexpr static std_out_<1> std_out;
constexpr static std_out_<2> std_err;

}

using boost::asio::buffer;
using boost::process::detail::close;
using boost::process::detail::null;
using boost::process::detail::std_out;
using boost::process::detail::std_err;
using boost::process::detail::std_in;

}}
#endif /* INCLUDE_BOOST_PROCESS_IO_HPP_ */
