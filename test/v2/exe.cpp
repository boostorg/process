// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process/v2/pid.hpp>
#include <boost/process/v2/exe.hpp>

#include <boost/test/unit_test.hpp>

#include <iostream>


BOOST_AUTO_TEST_CASE(test_exe)
{
    namespace bp2 = boost::process::v2;

    auto all = bp2::all_pids();
    for (unsigned i = 0; i < all.size(); i++) {
        auto pth = bp2::exe_path(all[i]);
        std::cout << ((!pth.string().empty()) ? pth.string() : "<unknown>") << std::endl;
    }
    
    // Check failed if all executable paths are unknown.
    BOOST_CHECK(std::count(all.begin(), all.end(), "<unknown>") != all.size());
}
