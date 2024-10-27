//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_PROCESS_BASIC_STREAM_INPUT_HPP
#define BOOST_PROCESS_BASIC_STREAM_INPUT_HPP

#include <boost/process/v2/detail/config.hpp>

BOOST_PROCESS_V2_BEGIN_NAMESPACE




template<typename Executor>
basic_stream_input<Executor> open_stdin(Executor executor, bool duplicate = true);

BOOST_PROCESS_V2_END_NAMESPACE

#endif //BOOST_PROCESS_BASIC_STREAM_INPUT_HPP
