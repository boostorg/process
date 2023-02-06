// Copyright (c) 2022 Klemens D. Morgenstern
// Copyright (c) 2022 Samuel Venable
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process/v2/pid.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <vector>

BOOST_AUTO_TEST_CASE(test_pid)
{
    namespace bp2 = boost::process::v2;
    BOOST_CHECK_NE(bp2::current_pid(), static_cast<bp2::pid_type>(0));

    auto all = bp2::all_pids();
    auto itr = std::find(all.begin(), all.end(), bp2::current_pid());

#if !defined(__APPLE___) && !defined(__MACH__)
    BOOST_CHECK_GT(all.size(), 0u);
    BOOST_CHECK(itr != all.end());

    std::vector<bp2::pid_type> children, grand_children; 
    auto grand_child_pids = [](bp2::pid_type pid, 
        std::vector<bp2::pid_type> & children, 
        std::vector<bp2::pid_type> & grand_children) 
    {  
        children = bp2::child_pids(pid);
        for (unsigned i = 0; i < children.size(); i++) 
        {
            std::vector<bp2::pid_type> tmp1;
            std::vector<bp2::pid_type> tmp2 = bp2::child_pids(children[i]);
            tmp1.insert(std::end(tmp1), std::begin(tmp2), std::end(tmp2));
            grand_children = tmp1;
        }
        return (!children.empty() || !grand_children.empty());
    };
    BOOST_CHECK_NE(grand_child_pids(bp2::root_pid, children, grand_children), false);
#endif

}
