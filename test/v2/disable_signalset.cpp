// Copyright (c) 2026 Stefan Dinkelacker
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Disable autolinking for unit tests.
#if !defined(BOOST_ALL_NO_LIB)
#define BOOST_ALL_NO_LIB 1
#endif // !defined(BOOST_ALL_NO_LIB)

// An application that handles SIGCHLD itself must keep asio from installing
// a handler for it.
#define BOOST_PROCESS_V2_DISABLE_SIGNALSET 1

#include <boost/process/v2/process.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/test/unit_test.hpp>

namespace bpv = boost::process::v2;
namespace asio = boost::asio;

BOOST_AUTO_TEST_SUITE(disable_signalset);

BOOST_AUTO_TEST_CASE(wait)
{
  using boost::unit_test::framework::master_test_suite;
  const auto pth = master_test_suite().argv[1];

  asio::io_context ctx;
  BOOST_CHECK_EQUAL(bpv::process(ctx, pth, {"exit-code", "42"}).wait(), 42);
}

BOOST_AUTO_TEST_CASE(async_wait)
{
  using boost::unit_test::framework::master_test_suite;
  const auto pth = master_test_suite().argv[1];

  asio::io_context ctx;
  bpv::process proc(ctx, pth, {"exit-code", "42"});

  bool called = false;
  boost::system::error_code ec;
  int code = -1;

  proc.async_wait(
      [&](boost::system::error_code ec_, int code_)
      {
        called = true;
        ec = ec_;
        code = code_;
      });

  ctx.run();
  BOOST_CHECK(called);

#if defined(BOOST_PROCESS_V2_WINDOWS) || defined(BOOST_PROCESS_V2_PIDFD_OPEN) \
 || defined(BOOST_PROCESS_V2_PDFORK) || defined(BOOST_PROCESS_V2_PIPEFORK)
  // These handles wait on a handle or descriptor, which needs no signal set.
  BOOST_CHECK_MESSAGE(!ec, ec.message());
  BOOST_CHECK_EQUAL(code, 42);
#else
  // The signal-based handle has nothing to wait on, but wait() still works.
  BOOST_CHECK_EQUAL(ec, asio::error::operation_not_supported);
  BOOST_CHECK_EQUAL(proc.wait(), 42);
#endif
}

BOOST_AUTO_TEST_SUITE_END();
