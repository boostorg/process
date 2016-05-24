// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PROCESS_PIPE_HPP
#define BOOST_PROCESS_PIPE_HPP

#include <boost/config.hpp>
#include <boost/process/detail/config.hpp>
#include <streambuf>
#include <istream>
#include <ostream>
#include <array>

#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/basic_pipe.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/basic_pipe.hpp>
#endif

namespace boost { namespace process {

using ::boost::process::detail::api::basic_pipe;

typedef basic_pipe<char>     pipe;
typedef basic_pipe<wchar_t> wpipe;

template<
    class CharT,
    class Traits = std::char_traits<CharT>
>
struct basic_pipebuf : std::basic_streambuf<CharT, Traits>
{
    typedef basic_pipe<CharT, Traits> pipe_type;

    typedef           CharT            char_type  ;
    typedef           Traits           traits_type;
    typedef  typename Traits::int_type int_type   ;
    typedef  typename Traits::pos_type pos_type   ;
    typedef  typename Traits::off_type off_type   ;


    basic_pipebuf()
    {
        this->setg(_read.begin(),  _read.begin()+ 10,  _read.begin() + 10);
        this->setp(_write.begin(), _write.begin());
    }
    basic_pipebuf(const basic_pipebuf & ) = default;
    basic_pipebuf(basic_pipebuf && ) = default;

    basic_pipebuf(pipe_type && p) : _pipe(std::move(p))
    {
        this->setg(_read.begin(),  _read.begin()+ 10,  _read.begin() + 10);
        this->setp(_write.begin(), _write.begin());
    }

    basic_pipebuf(const pipe_type & p) : _pipe(p)
    {
        this->setg(_read.begin(),  _read.begin()+ 10,  _read.begin() + 10);
        this->setp(_write.begin(), _write.begin());
    }

    basic_pipebuf& operator=(const basic_pipebuf & ) = default;
    basic_pipebuf& operator=(basic_pipebuf && ) = default;

    basic_pipebuf& operator=(pipe_type && p)
    {
        _pipe = std::move(p);
        return *this;
    }
    basic_pipebuf& operator=(const pipe_type & p)
    {
        _pipe = p;
        return *this;
    }

    int_type overflow(int_type ch = traits_type::eof()) override
    {
        if ((ch != traits_type::eof()) && _pipe.is_open())
        {
            *this->pptr() = ch;
            this->pbump(1);
            if (this->_write_impl())
                return ch;
        }
        return traits_type::eof();
    }
    int sync() override
    {
        return this->_write_impl() ? 0 : -1;
    }
    int_type underflow() override
    {
        if (!_pipe.is_open())
            return traits_type::eof();

        auto len = _read.end() - this->egptr() ;
        auto res = _pipe.read(this->egptr(), len);

        this->setg(this->eback(), this->gptr(), this->egptr() + res);
        auto val = *this->gptr();

        return  traits_type::to_int_type(val);
    }



    void pipe(pipe_type&& p)      {_pipe = std::move(p); }
    void pipe(const pipe_type& p) {_pipe = p; }
    const pipe_type &pipe() const {return _pipe;}
    pipe_type       &pipe()       {return _pipe;}
private:
    pipe_type _pipe;
    std::array<char_type, 1024> _write;
    std::array<char_type, 1024> _read;

    bool _write_impl()
    {
        if (!_pipe.is_open())
            return false;

        auto base = this->pbase();

        auto wrt = _pipe.write(base, this->pptr() - base);
        std::ptrdiff_t diff = this->pptr() - base;

        if (wrt < diff)
            std::move(base + wrt, base + diff, base);

        this->pbump(-wrt);

        return true;
    }
};

typedef basic_pipebuf<char>     pipebuf;
typedef basic_pipebuf<wchar_t> wpipebuf;

template<
    class CharT,
    class Traits = std::char_traits<CharT>
>
class basic_ipstream : public std::basic_istream<CharT, Traits>
{
    basic_pipebuf<CharT, Traits> _buf;
public:
    typedef basic_pipe<CharT, Traits> pipe_type;

    typedef           CharT            char_type  ;
    typedef           Traits           traits_type;
    typedef  typename Traits::int_type int_type   ;
    typedef  typename Traits::pos_type pos_type   ;
    typedef  typename Traits::off_type off_type   ;

    basic_pipebuf<CharT, Traits>* rdbuf() const {return _buf;};

    basic_ipstream() : std::basic_istream<CharT, Traits>(&_buf) {};
    basic_ipstream(const basic_ipstream & ) = delete;
    basic_ipstream(basic_ipstream && ) = default;

    basic_ipstream(pipe_type && p)      : std::basic_istream<CharT, Traits>(&_buf), _buf(std::move(p)) {}
    basic_ipstream(const pipe_type & p) : std::basic_istream<CharT, Traits>(&_buf), _buf(p) {}

    basic_ipstream& operator=(const basic_ipstream & ) = delete;
    basic_ipstream& operator=(basic_ipstream && ) = default;

    basic_ipstream& operator=(pipe_type && p)
    {
        _buf = std::move(p);
        return *this;
    }
    basic_ipstream& operator=(const pipe_type & p)
    {
        _buf = p;
        return *this;
    }
    void pipe(pipe_type&& p)      {_buf.pipe(std::move(p)); }
    void pipe(const pipe_type& p) {_buf.pipe(p); }
    const pipe_type &pipe() const {return _buf.pipe();}
    pipe_type       &pipe()       {return _buf.pipe();}
};

typedef basic_ipstream<char>     ipstream;
typedef basic_ipstream<wchar_t> wipstream;

template<
    class CharT,
    class Traits = std::char_traits<CharT>
>
class basic_opstream : public std::basic_ostream<CharT, Traits>
{
    basic_pipebuf<CharT, Traits> _buf;
public:
    typedef basic_pipe<CharT, Traits> pipe_type;

    typedef           CharT            char_type  ;
    typedef           Traits           traits_type;
    typedef  typename Traits::int_type int_type   ;
    typedef  typename Traits::pos_type pos_type   ;
    typedef  typename Traits::off_type off_type   ;


    basic_pipebuf<CharT, Traits>* rdbuf() const {return _buf;};

    basic_opstream() : std::basic_ostream<CharT, Traits>(&_buf)  {}
    basic_opstream(const basic_opstream & ) = delete;
    basic_opstream(basic_opstream && ) = default;

    basic_opstream(pipe_type && p)      : std::basic_ostream<CharT, Traits>(&_buf), _buf(std::move(p)) {}
    basic_opstream(const pipe_type & p) : std::basic_ostream<CharT, Traits>(&_buf), _buf(p) {}

    basic_opstream& operator=(const basic_opstream & ) = delete;
    basic_opstream& operator=(basic_opstream && ) = default;

    basic_opstream& operator=(pipe_type && p)
    {
        _buf = std::move(p);
        return *this;
    }
    basic_opstream& operator=(const pipe_type & p)
    {
        _buf = p;
        return *this;
    }
    void pipe(pipe_type&& p)      {_buf.pipe(std::move(p)); }
    void pipe(const pipe_type& p) {_buf.pipe(p); }
    const pipe_type &pipe() const {return _buf.pipe();}
    pipe_type       &pipe()       {return _buf.pipe();}
};

typedef basic_opstream<char>     opstream;
typedef basic_opstream<wchar_t> wopstream;

template<
    class CharT,
    class Traits = std::char_traits<CharT>
>
class basic_pstream : public std::basic_iostream<CharT, Traits>
{
    basic_pipebuf<CharT, Traits> _buf;
public:
    typedef basic_pipe<CharT, Traits> pipe_type;

    typedef           CharT            char_type  ;
    typedef           Traits           traits_type;
    typedef  typename Traits::int_type int_type   ;
    typedef  typename Traits::pos_type pos_type   ;
    typedef  typename Traits::off_type off_type   ;

    basic_pipebuf<CharT, Traits>* rdbuf() const {return _buf;};

    basic_pstream() : std::basic_iostream<CharT, Traits>(&_buf) {}
    basic_pstream(const basic_pstream & ) = delete;
    basic_pstream(basic_pstream && ) = default;

    basic_pstream(pipe_type && p)      : std::basic_iostream<CharT, Traits>(&_buf), _buf(std::move(p)) {}
    basic_pstream(const pipe_type & p) : std::basic_iostream<CharT, Traits>(&_buf), _buf(p) {}

    basic_pstream& operator=(const basic_pstream & ) = delete;
    basic_pstream& operator=(basic_pstream && ) = default;

    basic_pstream& operator=(pipe_type && p)
    {
        _buf = std::move(p);
        return *this;
    }
    basic_pstream& operator=(const pipe_type & p)
    {
        _buf = p;
        return *this;
    }
    void pipe(pipe_type&& p)      {_buf.pipe(std::move(p)); }
    void pipe(const pipe_type& p) {_buf.pipe(p); }
    const pipe_type &pipe() const {return _buf.pipe();}
    pipe_type       &pipe()       {return _buf.pipe();}
};

typedef basic_pstream<char>     pstream;
typedef basic_pstream<wchar_t> wpstream;



}}



#endif
