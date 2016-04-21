// Copyright (c) 2015 Klemens D. Morgenstern
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process.hpp>
#include <boost/program_options.hpp>

#include <vector>
#include <string>

#include <iostream>
#include <cstdint>

#include <chrono>


int main(int argc, char *argv[])
{
    using namespace std;
    using namespace boost::program_options;
    using namespace boost::process;


    bool launch_detached = false;
    bool launch_attached = false;

    options_description desc;
    desc.add_options()
         ("launch-detached", bool_switch(&launch_detached))
         ("launch-attached", bool_switch(&launch_attached))
         ;

    variables_map vm;
    command_line_parser parser(argc, argv);
    store(parser.options(desc).allow_unregistered().run(), vm);
    notify(vm);

    child c1;
    child c2;


    if (launch_attached)
    {
        c1 = execute(argv[0], attached);
#if defined ( BOOST_POSIX_API)
        auto h = c1.native_handle();
        cout << h << std::endl;
#elif defined ( BOOST_WINDOWS_API )
        auto h = reinterpret_cast<std::intptr_t>(c.native_handle());
        cout << h << endl;
#endif
    }


    if (launch_detached)
    {
        c2 = execute(argv[0]);
#if defined ( BOOST_POSIX_API)
        auto h = c2.native_handle();
        cout << h << std::endl;
#elif defined ( BOOST_WINDOWS_API )
        auto h = reinterpret_cast<std::intptr_t>(c.native_handle());
        cout << h << endl;
#endif
    }

    while (true)
        this_thread::sleep_for(chrono::milliseconds(100));


    return 0;
}
