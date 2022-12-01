// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/process/v2/pid.hpp>

#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_pid)
{
    namespace bp2 = boost::process::v2;
    BOOST_CHECK_NE(bp2::current_pid(), static_cast<bp2::pid_type>(0));

    auto all = bp2::all_pids();
    auto itr = std::find(all.begin(), all.end(), bp2::current_pid());
    BOOST_CHECK(itr != all.end());
    
    auto current_parent_pid = bp2::parent_pid(bp2::current_pid());
    BOOST_CHECK(!current_parent_pid.empty());

    std::vector<bp2::pid_type> children, grand_children; 
    auto grand_child_pids = [](bp2::pid_type pid, 
        std::vector<bp2::pid_type> & children, 
        std::vector<bp2::pid_type> & grand_children) 
    {  
        children.push_back(bp2::child_pids(pid));
        for (unsigned i = 0; i < children.size(); i++) 
        {
            grand_children.push_back(bp2::child_pids(children[i]));
        }
        return (!children.empty() && !grand_children.empty());
    }
    BOOST_CHECK(grand_child_pids(0));

}
