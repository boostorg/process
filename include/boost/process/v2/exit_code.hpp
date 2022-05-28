//
// process/exit_code.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2022 Klemens D. Morgenstern (klemens dot morgenstern at gmx dot net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_PROCESS_V2_EXIT_CODE_HPP
#define BOOST_PROCESS_V2_EXIT_CODE_HPP

#include <boost/process/v2/detail/config.hpp>

#if defined(BOOST_PROCESS_V2_POSIX)
#include <sys/wait.h>
#endif

BOOST_PROCESS_V2_BEGIN_NAMESPACE


#if defined(BOOST_PROCESS_V2_WINDOWS)

typedef unsigned long native_exit_code_type;

namespace detail
{
constexpr native_exit_code_type still_active = 259u;
}

inline bool process_is_running(native_exit_code_type code)
{
  return code == detail::still_active;
}

inline int evaluate_exit_code(native_exit_code_type code)
{
  return static_cast<int>(code);
}

#else

typedef int native_exit_code_type;

namespace detail
{
constexpr native_exit_code_type still_active = 0x7f;
}

inline bool process_is_running(int code)
{
    return !WIFEXITED(code) && !WIFSIGNALED(code);
}

inline int evaluate_exit_code(int code)
{
  if (WIFEXITED(code))
    return WEXITSTATUS(code);
  else if (WIFSIGNALED(code))
    return WTERMSIG(code);
  else
    return code;
}

#endif

BOOST_PROCESS_V2_END_NAMESPACE

#endif //BOOST_PROCESS_V2_EXIT_CODE_HPP