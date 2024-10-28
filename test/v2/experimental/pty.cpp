//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


#include <boost/process/v2/experimental/pty.hpp>
#include <boost/process/v2/process.hpp>

#include <boost/test/unit_test.hpp>

#include <boost/asio/coroutine.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>

#include <boost/asio/yield.hpp>

#if !defined(BOOST_PROCESS_V2_WINDOWS) || (NTDDI_VERSION >= 0x0A000006)

BOOST_AUTO_TEST_CASE(sync_plain)
{
  namespace bp2 = boost::process::v2;
  namespace net = boost::process::v2::net;
  using boost::unit_test::framework::master_test_suite;
  const auto pth = bp2::filesystem::absolute(master_test_suite().argv[1]);

  boost::asio::io_context ctx;
  bp2::experimental::pty pt{ctx};
  bp2::process proc(ctx, pth, {}, pt);


  char buf[4];
  net::read(pt, net::buffer(buf));

  std::string l1 = "Hello", l2 = ", ", l3 = "World!", l4 = "\n";

  std::string read_buffer;
  read_buffer.resize(64);
  auto rb = net::buffer(read_buffer);

  BOOST_CHECK_EQUAL(pt.write_some(net::buffer(l1)), 5);
  auto n = pt.read_some(rb);
  BOOST_CHECK_EQUAL(n,  5); rb += n;

  BOOST_CHECK_EQUAL(pt.write_some(net::buffer(l2)), 2);
  BOOST_CHECK_EQUAL(n = pt.read_some(rb),  2); rb += n;
  BOOST_CHECK_EQUAL(pt.write_some(net::buffer(l3)), 6);
  BOOST_CHECK_EQUAL(n = pt.read_some(rb),  6); rb += n;
  BOOST_CHECK_EQUAL(pt.write_some(net::buffer(l4)), 1);
  BOOST_CHECK_EQUAL(n = pt.read_some(rb),  2); rb += n;

  n = read_buffer.size() - rb.size();

  BOOST_CHECK_EQUAL(read_buffer.substr(0, n), "HELLO, WORLD!\r\n");
}

BOOST_AUTO_TEST_CASE(async_plain)
{
  namespace bp2 = boost::process::v2;
  namespace net = boost::process::v2::net;
  using boost::unit_test::framework::master_test_suite;
  const auto pth = bp2::filesystem::absolute(master_test_suite().argv[1]);

  boost::asio::io_context ctx;
  bp2::experimental::pty pt{ctx};
  bp2::process proc(ctx, pth, {"--async"}, pt);


  char buf[4];
  net::read(pt, net::buffer(buf));


  struct op : net::coroutine
  {
    bp2::experimental::pty &pt;
    bp2::process &proc;

    std::string read_buffer = std::string(64, ' ');
    net::mutable_buffer rb = net::buffer(read_buffer);
    std::string l1 = "Hello", l2 = ", ", l3 = "World!", l4 = "\n";


    void operator()(bp2::error_code ec, std::size_t n)
    {
      BOOST_REQUIRE_MESSAGE(!ec, ec.message());
      reenter(this)
      {
        yield pt.async_write_some(net::buffer(l1), *this);
        BOOST_CHECK_EQUAL(n, 5);
        yield pt.async_read_some(rb, *this);
        BOOST_CHECK_EQUAL(n,  5); rb += n;

        yield pt.async_write_some(net::buffer(l2), *this);
        BOOST_CHECK_EQUAL(n, 2);
        yield pt.async_read_some(rb, *this);
        BOOST_CHECK_EQUAL(n,  2);
        rb += n;
        yield pt.async_write_some(net::buffer(l3), *this);
        BOOST_CHECK_EQUAL(n, 6);
        yield pt.async_read_some(rb, *this);
        BOOST_CHECK_EQUAL(n, 6); rb += n;
        yield pt.async_write_some(net::buffer(l4), *this);
        BOOST_CHECK_EQUAL(n, 1);
        yield pt.async_read_some(rb, *this);
        BOOST_CHECK_EQUAL(n,  2); rb += n;

        n = read_buffer.size() - rb.size();
        BOOST_CHECK_EQUAL(read_buffer.substr(0, n), "HELLO, WORLD!\r\n");

        proc.terminate();

      }
    }
  };

  op{{}, pt, proc}({}, 0u);
  ctx.run();
}

BOOST_AUTO_TEST_CASE(sync_echo)
{
  namespace bp2 = boost::process::v2;
  namespace net = boost::process::v2::net;
  using boost::unit_test::framework::master_test_suite;
  const auto pth = bp2::filesystem::absolute(master_test_suite().argv[1]);

  boost::asio::io_context ctx;
  bp2::experimental::pty pt{ctx};
  bp2::process proc(ctx, pth, {"--echo"}, pt);


  char buf[4];
  net::read(pt, net::buffer(buf));

  std::string l1 = "Hello", l2 = ", ", l3 = "World!", l4 = "\n";

  std::string read_buffer;
  read_buffer.resize(64);
  auto rb = net::buffer(read_buffer);

  BOOST_CHECK_EQUAL(pt.write_some(net::buffer(l1)), 5);

  auto n = net::read(pt, net::buffer(rb, 10));
  BOOST_CHECK_EQUAL(n,  10);
  rb += n;

  BOOST_CHECK_EQUAL(pt.write_some(net::buffer(l2)), 2);
  BOOST_CHECK_EQUAL(n = net::read(pt, net::buffer(rb, 4)),  4); rb += n;
  BOOST_CHECK_EQUAL(pt.write_some(net::buffer(l3)), 6);
  BOOST_CHECK_EQUAL(n = net::read(pt, net::buffer(rb, 12)), 12); rb += n;
  BOOST_CHECK_EQUAL(pt.write_some(net::buffer(l4)), 1);
  BOOST_CHECK_EQUAL(n = net::read(pt, net::buffer(rb, 4)), 4); rb += n;

  n = read_buffer.size() - rb.size();

  BOOST_CHECK_EQUAL(read_buffer.substr(0, n), "HelloHELLO, , World!WORLD!^J\r\n");
}

BOOST_AUTO_TEST_CASE(async_echo)
{
  namespace bp2 = boost::process::v2;
  namespace net = boost::process::v2::net;
  using boost::unit_test::framework::master_test_suite;
  const auto pth = bp2::filesystem::absolute(master_test_suite().argv[1]);

  boost::asio::io_context ctx;
  bp2::experimental::pty pt{ctx};
  bp2::process proc(ctx, pth, {"--async", "--echo"}, pt);


  char buf[4];
  net::read(pt, net::buffer(buf));


  struct op : net::coroutine
  {
    bp2::experimental::pty &pt;
    bp2::process &proc;

    std::string read_buffer = std::string(64, ' ');
    net::mutable_buffer rb = net::buffer(read_buffer);
    std::string l1 = "Hello", l2 = ", ", l3 = "World!", l4 = "\n";


    void operator()(bp2::error_code ec, std::size_t n)
    {
      BOOST_REQUIRE_MESSAGE(!ec, ec.message());
      reenter(this)
      {
        yield pt.async_write_some(net::buffer(l1), *this);
        BOOST_CHECK_EQUAL(n, 5);
        yield net::async_read(pt, net::buffer(rb, 10), *this);
        BOOST_CHECK_EQUAL(n,  10); rb += n;

        yield pt.async_write_some(net::buffer(l2), *this);
        BOOST_CHECK_EQUAL(n, 2);
        yield net::async_read(pt, net::buffer(rb, 4), *this);
        BOOST_CHECK_EQUAL(n,  4);
        rb += n;
        yield pt.async_write_some(net::buffer(l3), *this);
        BOOST_CHECK_EQUAL(n, 6);
        yield net::async_read(pt, net::buffer(rb, 12), *this);
        BOOST_CHECK_EQUAL(n, 12); rb += n;
        yield pt.async_write_some(net::buffer(l4), *this);
        BOOST_CHECK_EQUAL(n, 1);
        yield net::async_read(pt, net::buffer(rb, 4), *this);
        BOOST_CHECK_EQUAL(n,  4); rb += n;

        n = read_buffer.size() - rb.size();
        BOOST_CHECK_EQUAL(read_buffer.substr(0, n), "HelloHELLO, , World!WORLD!^J\r\n");

        proc.terminate();

      }
    }
  };

  op{{}, pt, proc}({}, 0u);
  ctx.run();
}

BOOST_AUTO_TEST_CASE(async_line)
{
  namespace bp2 = boost::process::v2;
  namespace net = boost::process::v2::net;
  using boost::unit_test::framework::master_test_suite;
  const auto pth = bp2::filesystem::absolute(master_test_suite().argv[1]);

  boost::asio::io_context ctx;
  bp2::experimental::pty pt{ctx};
  bp2::process proc(ctx, pth, {"--async", "--line"}, pt);


  char buf[4];
  net::read(pt, net::buffer(buf));


  std::size_t read = 0u;
  std::string read_buffer;
  net::async_read_until(
      pt, net::dynamic_buffer(read_buffer), '\n',
      [&](bp2::error_code ec, std::size_t n)
      {
        BOOST_CHECK_EQUAL(read_buffer.substr(0, n), "HELLO, WORLD!\r\n");
        read = n;
      });

  struct op : net::coroutine
  {
    bp2::experimental::pty &pt;
    std::size_t & read;

    std::string l1 = "Hello", l2 = ", ", l3 = "World!", l4 = "\n";


    void operator()(bp2::error_code ec, std::size_t n)
    {
      BOOST_REQUIRE_MESSAGE(!ec, ec.message());
      reenter(this)
      {
        yield pt.async_write_some(net::buffer(l1), *this);
        BOOST_CHECK_EQUAL(n, 5);
        BOOST_CHECK_EQUAL(read, 0);
        yield pt.async_write_some(net::buffer(l2), *this);
        BOOST_CHECK_EQUAL(n, 2);
        BOOST_CHECK_EQUAL(read, 0);
        yield pt.async_write_some(net::buffer(l3), *this);
        BOOST_CHECK_EQUAL(n, 6);
        BOOST_CHECK_EQUAL(read, 0);
        yield pt.async_write_some(net::buffer(l4), *this);
        BOOST_CHECK_EQUAL(n, 1);
        BOOST_CHECK_EQUAL(read, 0);
      }
    }
  };

  op{{}, pt, read}({}, 0u);
  ctx.run();
}


BOOST_AUTO_TEST_CASE(sync_line_echo)
{
  namespace bp2 = boost::process::v2;
  namespace net = boost::process::v2::net;
  using boost::unit_test::framework::master_test_suite;
  const auto pth = bp2::filesystem::absolute(master_test_suite().argv[1]);

  boost::asio::io_context ctx;
  bp2::experimental::pty pt{ctx};
  bp2::process proc(ctx, pth, {"--line", "--echo"}, pt);


  char buf[4];
  net::read(pt, net::buffer(buf));

  std::string l1 = "Hello", l2 = ", ", l3 = "World!", l4 = "\n";

  std::string read_buffer;
  read_buffer.resize(64);
  auto rb = net::buffer(read_buffer);

  BOOST_CHECK_EQUAL(pt.write_some(net::buffer(l1)), 5);
  auto n = pt.read_some(rb);
  BOOST_CHECK_EQUAL(n,  5); rb += n;

  BOOST_CHECK_EQUAL(pt.write_some(net::buffer(l2)), 2);
  BOOST_CHECK_EQUAL(n = pt.read_some(rb),  2); rb += n;
  BOOST_CHECK_EQUAL(pt.write_some(net::buffer(l3)), 6);
  BOOST_CHECK_EQUAL(n = pt.read_some(rb),  6); rb += n;
  BOOST_CHECK_EQUAL(pt.write_some(net::buffer(l4)), 1);
  BOOST_CHECK_EQUAL(n = pt.read_some(rb),  2); rb += n;


  rb += pt.read_some(rb);
  n = read_buffer.size() - rb.size();

  BOOST_CHECK_EQUAL(read_buffer.substr(0, n), "Hello, World!\r\nHELLO, WORLD!\r\n");
}


BOOST_AUTO_TEST_CASE(resize)
{
  namespace bp2 = boost::process::v2;
  namespace net = boost::process::v2::net;
  using boost::unit_test::framework::master_test_suite;
  const auto pth = bp2::filesystem::absolute(master_test_suite().argv[1]);

  boost::asio::io_context ctx;
  bp2::experimental::pty pt{ctx};

  bp2::experimental::console_size_t cs{100, 40};
  pt.open(cs);

  bp2::process proc(ctx, pth, {"--wait-resize"}, pt);


  char buf[4];
  net::read(pt, net::buffer(buf));

  std::string line;
  auto n = net::read_until(pt, net::dynamic_buffer(line), '\n');
  BOOST_CHECK_EQUAL(line, "cols=100 rows=40\r\n");

  line.clear();

  cs.columns = 85;
  cs.rows    = 35;
  pt.resize(cs);

  n = net::read_until(pt, net::dynamic_buffer(line), '\n');
  BOOST_CHECK_EQUAL(line, "cols=85 rows=35\r\n");
}

#endif