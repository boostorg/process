// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

using namespace boost::process;
using namespace boost::iostreams;

int main()
{
//[stdout
    system("test.exe", std_out > "log.txt");
//]

//[close_in_err
    system("test.exe",
		std_out > null,
		std_in  < null,
		std_err.close());
//]
}
