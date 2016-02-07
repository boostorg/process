// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZERS_SET_ARGS_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZERS_SET_ARGS_HPP

#include <boost/process/windows/initializers/initializer_base.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/shared_array.hpp>
#include <sstream>

namespace boost { namespace process { namespace windows { namespace initializers {

template <class Range>
struct set_args_ : initializer_base
{
    typedef typename Range::const_iterator const_iterator;
    typedef typename Range::value_type string_type;
    typedef typename string_type::value_type char_type;
    typedef std::basic_ostringstream<char_type> ostringstream;

    explicit set_args_(const Range &args)
    {
        const_iterator it = boost::const_begin(args);
        const_iterator end = boost::const_end(args);
        if (it != end)
        {
            exe_ = *it;
            ostringstream os;
            for (; it != end; ++it)
            {
                if (boost::algorithm::contains(*it,
                    string_type(1, static_cast<char_type>(' '))))
                {
                    os << static_cast<char_type>('"') << *it <<
                        static_cast<char_type>('"');
                }
                else
                {
                    os << *it;
                }
                os << static_cast<char_type>(' ');
            }
            string_type s = os.str();
            cmd_line_.reset(new char_type[s.size() + 1]);
            boost::copy(s, cmd_line_.get());
            cmd_line_[s.size()] = 0;
        }
        else
        {
            cmd_line_.reset(new char_type[1]());
        }
    }

    template <class WindowsExecutor>
    void on_CreateProcess_setup(WindowsExecutor &e) const
    {
        e.cmd_line = cmd_line_.get();
        if (!e.exe && !exe_.empty())
            e.exe = exe_.c_str();
    }

private:
    boost::shared_array<char_type> cmd_line_;
    string_type exe_;
};

template <class Range>
set_args_<Range> set_args(const Range &range)
{
    return set_args_<Range>(range);
}

}}}}

#endif
