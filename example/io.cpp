// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process.hpp>
#include <boost/iostreams/stream.hpp>
#include <string>

using namespace boost::process;

int main()
{
//[close
    system(
        "test.exe",
        std_out.close(),
        std_err.close(),
        std_in.close()
    );
//]
//[null
    system(
        "test.exe",
        (std_out & std_err) > null, 
        std_in < null
    );
//]
//[redirect
    boost::filesystem::path p = "input.txt";
    system(
        "test.exe",
        std_err > "log.txt",
        std_out > stderr,
        std_in < p
    );
//]
    {
//[pipe
    pipe p1;
    pipe p2;
    system(
        "test.exe",
        std_out > p2,
        std_in < p1
    );
    auto text = "my_text";
    p1.write(text, 8);
    boost::iostreams::stream<boost::iostreams::file_descriptor_source> istr(p2.source());
    
    int i = 0;
    p2 >> i;

//]
    }
    {
//[async_pipe
    boost::asio::io_service io_service;
    async_pipe p1(io_service);
    async_pipe p2(io_service);
    system(
        "test.exe",
        std_out > p2,
        std_in < p1,
        io_service,
        on_exit([&](int exit, const std::error_code& ec_in)
            {
                p1.async_close();
                p2.async_close();
            })
    );
    std::vector<char> in_buf;
    std::string value = "my_string";
    boost::asio::async_write(p1, boost::asio::buffer(value),  []( const boost::system::error_code&, std::size_t){});
    boost::asio::async_read (p2, boost::asio::buffer(in_buf), []( const boost::system::error_code&, std::size_t){});
//]
    }{
//[async_pipe_simple
    	boost::asio::io_service io_service;
    std::vector<char> in_buf;
    std::string value = "my_string";
    system(
        "test.exe",
        std_out > buffer(in_buf),
        std_in  < buffer(value)    
        );
//]
    }{
//[async_pipe_future
	boost::asio::io_service io_service;
    std::future<std::vector<char>> in_buf;
    std::future<void> write_fut;
    std::string value = "my_string";
    system(
        "test.exe",
        std_out > in_buf,
        std_in  < buffer(value) > write_fut 
        );

    write_fut.get();
    in_buf.get();
//]
    }
}
