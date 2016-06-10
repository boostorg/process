// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#if defined(BOOST_WINDOWS_API)
#   include <Windows.h>
#endif

using namespace boost::process;

int main()
{
//[async_io
    boost::asio::io_service ios;
    boost::process::async_pipe p(ios);

    child c(
        "test.exe",
        std_out > ios
    );

    boost::array<char, 4096> buffer;
    boost::asio::async_read(p, boost::asio::buffer(buffer),
        [](const boost::system::error_code&, std::size_t){});

    ios.run();
//]
}
