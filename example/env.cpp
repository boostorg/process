// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process.hpp>

using namespace boost::process;

int main()
{
//[modifiy_env
	boost::process::environment my_env = boost::this_process::environment(); //empty env, that would fail.
	execute("test.exe", my_env);
//]
//[inherit_env
    execute("test.exe", env["PATH"]+="/tmp");
//]
}
