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
#include <vector>

#if defined(BOOST_POSIX_API)
#include <boost/process/detail/posix/basic_pipe.hpp>
#elif defined(BOOST_WINDOWS_API)
#include <boost/process/detail/windows/basic_pipe.hpp>
#endif

namespace boost { namespace process {

using ::boost::process::detail::api::basic_pipe;

#if defined(BOOST_PROCESS_DOXYGEN)
/** Class implementation of a pipe.
 *
 */
template<class CharT, class Traits = std::char_traits<CharT>>
class basic_pipe
{
public:
    typedef CharT                      char_type  ;
    typedef          Traits            traits_type;
    typedef typename Traits::int_type  int_type   ;
    typedef typename Traits::pos_type  pos_type   ;
    typedef typename Traits::off_type  off_type   ;
    typedef ::boost::detail::winapi::HANDLE_ native_handle;

    /// Default construct the pipe. Will be opened.
    basic_pipe();

    ///Construct a named pipe.
    inline explicit basic_pipe(const std::string & name);
    /** Copy construct the pipe.
     *  \note Duplicated the handles.
     */
    inline basic_pipe(const basic_pipe& p);
    /** Move construct the pipe. */
    basic_pipe(basic_pipe&& lhs);
    /** Copy assign the pipe.
     *  \note Duplicated the handles.
     */
    inline basic_pipe& operator=(const basic_pipe& p);
    /** Move assign the pipe. */
    basic_pipe& operator=(basic_pipe&& lhs);
    /** Destructor closes the handles. */
    ~basic_pipe();
    /** Get the native handle of the source. */
    native_handle native_source() const;
    /** Get the native handle of the sink. */
    native_handle native_sink  () const;

    ///Write date to the pipe.
    int_type write(const char_type * data, int_type count);
    ///Read date from the pipe.
    int_type read(char_type * data, int_type count);
    ///Check if the pipe is open.
    bool is_open();
    ///Close the pipe
    void close();
};

#endif



typedef basic_pipe<char>     pipe;
typedef basic_pipe<wchar_t> wpipe;


/** Implementation of the stream buffer for a pipe.
 */
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

    ///Default constructor, will also construct the pipe.
    basic_pipebuf() : _write(1024), _read(1024)
    {
        this->setg(_read.data(),  _read.data()+ 10,  _read.data() + 10);
        this->setp(_write.data(), _write.data());
    }
    ///Copy Constructor.
    basic_pipebuf(const basic_pipebuf & ) = default;
    ///Move Constructor
    basic_pipebuf(basic_pipebuf && ) = default;

    ///Move construct from a pipe.
    basic_pipebuf(pipe_type && p) : _pipe(std::move(p)), _write(1024), _read(1024)
    {
        this->setg(_read.data(),  _read.data()+ 10,  _read.data() + 10);
        this->setp(_write.data(), _write.data());
    }
    ///Construct from a pipe.
    basic_pipebuf(const pipe_type & p) : _pipe(p), _write(1024), _read(1024)
    {
        this->setg(_read.data(),  _read.data()+ 10,  _read.data() + 10);
        this->setp(_write.data(), _write.data());
    }

    ///Copy assign.
    basic_pipebuf& operator=(const basic_pipebuf & ) = delete;
    ///Move assign.
    basic_pipebuf& operator=(basic_pipebuf && ) = default;
    ///Move assign a pipe.
    basic_pipebuf& operator=(pipe_type && p)
    {
        _pipe = std::move(p);
        return *this;
    }
    ///Copy assign a pipe.
    basic_pipebuf& operator=(const pipe_type & p)
    {
        _pipe = p;
        return *this;
    }
    ///writes characters to the associated output sequence from the put area
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
    ///synchronizes the buffers with the associated character sequence
    int sync() override
    {
        return this->_write_impl() ? 0 : -1;
    }
    ///reads characters from the associated input sequence to the get area
    int_type underflow() override
    {
        if (!_pipe.is_open())
            return traits_type::eof();

        if (this->egptr() == &_read.back()) //ok, so we're at the end of the buffer
            this->setg(_read.data(),  _read.data()+ 10,  _read.data() + 10);


        auto len = &_read.back() - this->egptr() ;
        auto res = _pipe.read(this->egptr(), len);


        this->setg(this->eback(), this->gptr(), this->egptr() + res);
        auto val = *this->gptr();

        return  traits_type::to_int_type(val);
    }


    ///Set the pipe of the streambuf.
    void pipe(pipe_type&& p)      {_pipe = std::move(p); }
    ///Set the pipe of the streambuf.
    void pipe(const pipe_type& p) {_pipe = p; }
    ///Get a const reference of the pipe.
    const pipe_type &pipe() const {return _pipe;}
    ///Get a reference of the pipe. \note Is a reference so it can be moved.
    pipe_type       &pipe()       {return _pipe;}
private:
    pipe_type _pipe;
    std::vector<char_type> _write;
    std::vector<char_type> _read;

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

/** Implementation of a reading pipe stream.
 *
 */
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

    ///Get access to the underlying stream_buf
    basic_pipebuf<CharT, Traits>* rdbuf() const {return _buf;};

    ///Default constructor.
    basic_ipstream() : std::basic_istream<CharT, Traits>(&_buf) {};
    ///Copy constructor.
    basic_ipstream(const basic_ipstream & ) = delete;
    ///Move constructor.
    basic_ipstream(basic_ipstream && ) = default;

    ///Move construct from a pipe.
    basic_ipstream(pipe_type && p)      : std::basic_istream<CharT, Traits>(&_buf), _buf(std::move(p)) {}

    ///Copy construct from a pipe.
    basic_ipstream(const pipe_type & p) : std::basic_istream<CharT, Traits>(&_buf), _buf(p) {}

    ///Copy assignment.
    basic_ipstream& operator=(const basic_ipstream & ) = delete;
    ///Move assignment
    basic_ipstream& operator=(basic_ipstream && ) = default;
    ///Move assignment of a pipe.
    basic_ipstream& operator=(pipe_type && p)
    {
        _buf = std::move(p);
        return *this;
    }
    ///Copy assignment of a pipe.
    basic_ipstream& operator=(const pipe_type & p)
    {
        _buf = p;
        return *this;
    }
    ///Set the pipe of the streambuf.
    void pipe(pipe_type&& p)      {_buf.pipe(std::move(p)); }
    ///Set the pipe of the streambuf.
    void pipe(const pipe_type& p) {_buf.pipe(p); }
    ///Get a const reference of the pipe.
    const pipe_type &pipe() const {return _buf.pipe();}
    ///Get a reference of the pipe. \note Is a reference so it can be moved.
    pipe_type       &pipe()       {return _buf.pipe();}
};

typedef basic_ipstream<char>     ipstream;
typedef basic_ipstream<wchar_t> wipstream;

/** Implementation of a write pipe stream.
 *
 */
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


    ///Get access to the underlying stream_buf
    basic_pipebuf<CharT, Traits>* rdbuf() const {return _buf;};

    ///Default constructor.
    basic_opstream() : std::basic_ostream<CharT, Traits>(&_buf) {};
    ///Copy constructor.
    basic_opstream(const basic_opstream & ) = delete;
    ///Move constructor.
    basic_opstream(basic_opstream && ) = default;

    ///Move construct from a pipe.
    basic_opstream(pipe_type && p)      : std::basic_ostream<CharT, Traits>(&_buf), _buf(std::move(p)) {}

    ///Copy construct from a pipe.
    basic_opstream(const pipe_type & p) : std::basic_ostream<CharT, Traits>(&_buf), _buf(p) {}

    ///Copy assignment.
    basic_opstream& operator=(const basic_opstream & ) = delete;
    ///Move assignment
    basic_opstream& operator=(basic_opstream && ) = default;
    ///Move assignment of a pipe.
    basic_opstream& operator=(pipe_type && p)
    {
        _buf = std::move(p);
        return *this;
    }
    ///Copy assignment of a pipe.
    basic_opstream& operator=(const pipe_type & p)
    {
        _buf = p;
        return *this;
    }
    ///Set the pipe of the streambuf.
    void pipe(pipe_type&& p)      {_buf.pipe(std::move(p)); }
    ///Set the pipe of the streambuf.
    void pipe(const pipe_type& p) {_buf.pipe(p); }
    ///Get a const reference of the pipe.
    const pipe_type &pipe() const {return _buf.pipe();}
    ///Get a reference of the pipe. \note Is a reference so it can be moved.
    pipe_type       &pipe()       {return _buf.pipe();}
};

typedef basic_opstream<char>     opstream;
typedef basic_opstream<wchar_t> wopstream;


/** Implementation of a read-write pipe stream.
 *
 */
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


    ///Get access to the underlying stream_buf
    basic_pipebuf<CharT, Traits>* rdbuf() const {return _buf;};

    ///Default constructor.
    basic_pstream() : std::basic_iostream<CharT, Traits>(&_buf) {};
    ///Copy constructor.
    basic_pstream(const basic_pstream & ) = delete;
    ///Move constructor.
    basic_pstream(basic_pstream && ) = default;

    ///Move construct from a pipe.
    basic_pstream(pipe_type && p)      : std::basic_iostream<CharT, Traits>(&_buf), _buf(std::move(p)) {}

    ///Copy construct from a pipe.
    basic_pstream(const pipe_type & p) : std::basic_iostream<CharT, Traits>(&_buf), _buf(p) {}

    ///Copy assignment.
    basic_pstream& operator=(const basic_pstream & ) = delete;
    ///Move assignment
    basic_pstream& operator=(basic_pstream && ) = default;
    ///Move assignment of a pipe.
    basic_pstream& operator=(pipe_type && p)
    {
        _buf = std::move(p);
        return *this;
    }
    ///Copy assignment of a pipe.
    basic_opstream& operator=(const pipe_type & p)
    {
        _buf = p;
        return *this;
    }
    ///Set the pipe of the streambuf.
    void pipe(pipe_type&& p)      {_buf.pipe(std::move(p)); }
    ///Set the pipe of the streambuf.
    void pipe(const pipe_type& p) {_buf.pipe(p); }
    ///Get a const reference of the pipe.
    const pipe_type &pipe() const {return _buf.pipe();}
    ///Get a reference of the pipe. \note Is a reference so it can be moved.
    pipe_type       &pipe()       {return _buf.pipe();}
};

typedef basic_pstream<char>     pstream;
typedef basic_pstream<wchar_t> wpstream;



}}



#endif
