// Copyright (c) 2022 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


// Disable autolinking for unit tests.
#if !defined(BOOST_ALL_NO_LIB)
#define BOOST_ALL_NO_LIB 1
#endif // !defined(BOOST_ALL_NO_LIB)

#define BOOST_TEST_IGNORE_SIGCHLD 1

#if  true //defined(BOOST_POSIX_API)
#   include <signal.h>
#endif
// Test that header file is self-contained.
#include <boost/process/v2/popen.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/process/v2/environment.hpp>
#include <boost/process/v2/start_dir.hpp>
#include <boost/process/v2/execute.hpp>
#include <boost/process/v2/stdio.hpp>
#include <boost/process/v2/bind_launcher.hpp>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <boost/process/v2/windows/creation_flags.hpp>
#include <boost/process/v2/windows/show_window.hpp>
#endif

#include <boost/test/unit_test.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/connect_pipe.hpp>
#include <boost/asio/cancel_after.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/readable_pipe.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/writable_pipe.hpp>

#include <fstream>
#include <thread>

namespace bpv = boost::process::v2;
namespace asio = boost::asio;


BOOST_AUTO_TEST_SUITE(with_target);

BOOST_AUTO_TEST_CASE(exit_code_sync)
{
    using boost::unit_test::framework::master_test_suite;
    const auto pth =  master_test_suite().argv[1];

    boost::asio::io_context ctx;

    BOOST_CHECK_EQUAL(bpv::process(ctx, pth, {"exit-code", "0"}).wait(), 0);
    BOOST_CHECK_EQUAL(bpv::execute(bpv::process(ctx, pth, {"exit-code", "1"})), 1);
    std::vector<std::string> args = {"exit-code", "2"};
    BOOST_CHECK_EQUAL(bpv::default_process_launcher()(ctx, pth, args).wait(), 2);
    args[1] = "42";
    auto proc = bpv::default_process_launcher()(ctx, pth, args);
    BOOST_CHECK_EQUAL(proc.wait(), 42);

    BOOST_CHECK_EQUAL(bpv::process(ctx, pth, {"sleep", "100"}).wait(), 0);
    BOOST_CHECK_EQUAL(bpv::execute(bpv::process(ctx, pth, {"sleep", "100"})), 0);
}

BOOST_AUTO_TEST_CASE(exit_code_async)
{
    using boost::unit_test::framework::master_test_suite;
    printf("Running exit_code_async\n");
    auto & mm = master_test_suite();
    printf("Running exit_code_async %p\n", static_cast<void*>(&mm));
    printf("Args: '%d'\n", master_test_suite().argc);
    printf("Exe '%s'\n", master_test_suite().argv[0]);
    const auto pth =  master_test_suite().argv[1];
    printf("Executing '%s'\n", pth);

    boost::asio::io_context ctx;

    int called = 0;
    printf("Setting up processes\n");

    bpv::process proc1(ctx, pth, {"exit-code", "0"});
    bpv::process proc3(ctx, pth, {"exit-code", "2"});
    bpv::process proc4(ctx, pth, {"exit-code", "42"});
    bpv::process proc5(ctx, pth, {"sleep", "100"});
    bpv::process proc6(ctx, pth, {"sleep", "50"});

#define CPL(Code)                                          \
    [&](bpv::error_code ec, int e)                         \
    {                                                      \
      BOOST_CHECK_MESSAGE(!ec, ec.message());              \
      called++;                                            \
      BOOST_CHECK_EQUAL(bpv::evaluate_exit_code(e), Code); \
    }

    printf("Waiting for processes\n");

    proc1.async_wait(CPL(0));
    proc3.async_wait(CPL(2));
    proc4.async_wait(CPL(42));
    proc5.async_wait(CPL(0));
    proc6.async_wait(CPL(0));
    bpv::async_execute(bpv::process(ctx, pth, {"exit-code", "1"}),CPL(1));
    bpv::async_execute(bpv::process(ctx, pth, {"sleep", "100"}),  CPL(0));

#undef CPL

    printf("Running\n");

    ctx.run();
    BOOST_CHECK_EQUAL(called, 7);
}


BOOST_AUTO_TEST_CASE(terminate)
{
  asio::io_context ctx;
  using boost::unit_test::framework::master_test_suite;
  const auto pth =  master_test_suite().argv[1];


  bpv::process proc(ctx, pth, {"sleep", "10"});
  proc.suspend();
  proc.resume();
  boost::system::error_code ec;
  proc.terminate(ec);
  proc.wait(ec);
  BOOST_WARN_NE(proc.exit_code(), 0);
}

BOOST_AUTO_TEST_CASE(request_exit)
{
  asio::io_context ctx;

  using boost::unit_test::framework::master_test_suite;
  const auto pth =  master_test_suite().argv[1];


  bpv::process proc(ctx, pth, {"sigterm"}
#if defined(BOOST_PROCESS_V2_WINDOWS)
    , bpv::windows::show_window_minimized_not_active
    , bpv::windows::create_new_console
#endif
    );
  BOOST_CHECK(proc.running());
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  proc.request_exit();
  proc.wait();
  BOOST_CHECK_EQUAL(proc.exit_code() & ~SIGTERM, 0);
}

bool can_interrupt = true;

BOOST_AUTO_TEST_CASE(interrupt)
{
  asio::io_context ctx;
  using boost::unit_test::framework::master_test_suite;
  const auto pth =  master_test_suite().argv[1];


  bpv::process proc(ctx, pth, {"sigint"}
#if defined(BOOST_PROCESS_V2_WINDOWS)
  , bpv::windows::create_new_process_group
#endif
  );
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  bpv::error_code ec;
  proc.interrupt(ec);

#if defined(BOOST_PROCESS_V2_WINDOWS)
  // the interrupt only works on console applications, so it may not work depending on the environment.
  if (ec.value() == ERROR_INVALID_FUNCTION)
  {
    can_interrupt = false;
    return;
  }
#endif

  BOOST_CHECK_MESSAGE(!ec, ec.what());
  proc.wait();
  BOOST_CHECK_EQUAL(proc.exit_code() & ~SIGTERM, 0);
}

void trim_end(std::string & str)
{
    auto itr = std::find_if(str.rbegin(), str.rend(), &std::char_traits<char>::not_eof);
    str.erase(itr.base(), str.end());
    if (!str.empty() && str.back() == '\r')
      str.pop_back();
}

BOOST_AUTO_TEST_CASE(print_args_out)
{
  using boost::unit_test::framework::master_test_suite;
  const auto pth =  master_test_suite().argv[1];

  asio::io_context ctx;

  asio::readable_pipe rp{ctx};
  asio::writable_pipe wp{ctx};
  asio::connect_pipe(rp, wp);


  bpv::process proc(ctx, pth, {"print-args", "foo", "bar"}, bpv::process_stdio{/*in*/{},/*out*/wp, /*err*/ nullptr});

  wp.close();
  asio::streambuf st;
  std::istream is{&st};
  bpv::error_code ec;

  auto sz = asio::read(rp, st,  ec);
  while (ec == asio::error::interrupted)
      sz += asio::read(rp, st,  ec);

  BOOST_CHECK_NE(sz, 0u);
  BOOST_CHECK_MESSAGE((ec == asio::error::broken_pipe) || (ec == asio::error::eof), ec.message());

  std::string line;
  BOOST_CHECK(std::getline(is, line));
  trim_end(line);
  BOOST_CHECK_EQUAL(pth, line);

  BOOST_CHECK(std::getline(is, line));
  trim_end(line);
  BOOST_CHECK_EQUAL("print-args", line);

  BOOST_CHECK(std::getline(is, line));
  trim_end(line);
  BOOST_CHECK_EQUAL("foo", line);

  BOOST_CHECK(std::getline(is, line));
  trim_end(line);
  BOOST_CHECK_EQUAL("bar", line);


  proc.wait();
  BOOST_CHECK(proc.exit_code() == 0);
}


BOOST_AUTO_TEST_CASE(print_args_err)
{
  using boost::unit_test::framework::master_test_suite;
  const auto pth =  master_test_suite().argv[1];

  asio::io_context ctx;

  asio::readable_pipe rp{ctx};
  asio::writable_pipe wp{ctx};
  asio::connect_pipe(rp, wp);

  bpv::process proc(ctx, pth, {"print-args", "bar", "foo"}, bpv::process_stdio{/*in*/{}, /*.out= */ nullptr, /* .err=*/ wp});

  wp.close();
  asio::streambuf st;
  std::istream is{&st};
  bpv::error_code ec;

  auto sz = asio::read(rp, st,  ec);
  while (ec == asio::error::interrupted)
      sz += asio::read(rp, st,  ec);

  BOOST_CHECK_NE(sz , 0u);
  BOOST_CHECK_MESSAGE((ec == asio::error::broken_pipe) || (ec == asio::error::eof), ec.message());

  std::string line;
  BOOST_CHECK(std::getline(is, line));
  trim_end(line);
  BOOST_CHECK_EQUAL(pth, line );

  BOOST_CHECK(std::getline(is, line));
  trim_end(line);
  BOOST_CHECK_EQUAL("print-args", line);

  BOOST_CHECK(std::getline(is, line));
  trim_end(line);
  BOOST_CHECK_EQUAL("bar", line);

  BOOST_CHECK(std::getline(is, line));
  trim_end(line);
  BOOST_CHECK_EQUAL("foo", line);


  proc.wait();
  BOOST_CHECK_EQUAL(proc.exit_code(), 0);
}

BOOST_AUTO_TEST_CASE(echo_file)
{
  using boost::unit_test::framework::master_test_suite;
  const auto pth =  master_test_suite().argv[1];

  asio::io_context ctx;

  asio::readable_pipe rp{ctx};
  asio::writable_pipe wp{ctx};
  asio::connect_pipe(rp, wp);

  auto p = bpv::filesystem::temp_directory_path() / "asio-test-thingy.txt";

  std::string test_data = "some ~~ test ~~ data";
  {
    std::ofstream ofs{p.string()};
    ofs.write(test_data.data(), test_data.size());
    BOOST_CHECK(ofs);
  }

  bpv::process proc(ctx, pth, {"echo"}, bpv::process_stdio{/*.in=*/p, /*.out=*/wp, /*.err*/{}});
  wp.close();

  std::string out;
  bpv::error_code ec;

  auto sz = asio::read(rp, asio::dynamic_buffer(out),  ec);
  while (ec == asio::error::interrupted)
      sz += asio::read(rp, asio::dynamic_buffer(out),  ec);

  BOOST_CHECK(sz != 0);
  BOOST_CHECK_MESSAGE((ec == asio::error::broken_pipe) || (ec == asio::error::eof), ec.message());
  BOOST_CHECK_MESSAGE(out == test_data, out);

  proc.wait();
  BOOST_CHECK_MESSAGE(proc.exit_code() == 0, proc.exit_code());
}

BOOST_AUTO_TEST_CASE(stdio_creates_complementary_pipes)
{
  using boost::unit_test::framework::master_test_suite;
  const auto pth =  master_test_suite().argv[1];

  asio::io_context ctx;

  asio::readable_pipe rp{ctx};
  asio::writable_pipe wp{ctx};
  // Pipes intentionally not connected. `process_stdio` will create pipes
  // complementing both of these and retains ownership of those pipes.

  bpv::process proc(ctx, pth, {"echo"}, bpv::process_stdio{/*.in=*/wp, /*.out=*/rp, /*.err=*/nullptr});

  asio::write(wp, asio::buffer("foo", 3));
  asio::write(wp, asio::buffer("bar", 3));
  wp.close();

  bpv::error_code ec;
  std::string out;
  auto sz = asio::read(rp, asio::dynamic_buffer(out),  ec);
  while (ec == asio::error::interrupted)
      sz += asio::read(rp, asio::dynamic_buffer(out),  ec);
  BOOST_CHECK_EQUAL(sz, 6u);
  BOOST_CHECK_MESSAGE((ec == asio::error::broken_pipe) || (ec == asio::error::eof), ec.message());
  BOOST_CHECK_EQUAL(out, "foobar");

  proc.wait();
  BOOST_CHECK(proc.exit_code() == 0);
}

BOOST_AUTO_TEST_CASE(stdio_move_semantics)
{
  using boost::unit_test::framework::master_test_suite;
  const auto pth =  master_test_suite().argv[1];

  asio::io_context ctx;

  asio::readable_pipe rp{ctx};
  asio::writable_pipe wp{ctx};

  auto make_stdio = [&]() -> bpv::process_stdio {
      bpv::process_stdio stdio{};
      stdio.in = wp;
      stdio.out = rp;
      stdio.err = nullptr;
      // intentionally pessimizing move, preventing NRVO
      return std::move(stdio);
  };
  bpv::process proc(ctx, pth, {"echo"}, make_stdio());

  bpv::error_code ec;
  asio::write(wp, asio::buffer("foobar", 6), ec);
  BOOST_CHECK_MESSAGE(!ec, ec.message());
  wp.close();

  std::string out;
  auto sz = asio::read(rp, asio::dynamic_buffer(out),  ec);
  while (ec == asio::error::interrupted)
      sz += asio::read(rp, asio::dynamic_buffer(out),  ec);
  BOOST_CHECK_EQUAL(sz, 6u);
  BOOST_CHECK_MESSAGE((ec == asio::error::broken_pipe) || (ec == asio::error::eof), ec.message());
  BOOST_CHECK_EQUAL(out, "foobar");

  proc.wait();
  BOOST_CHECK(proc.exit_code() == 0);
}

BOOST_AUTO_TEST_CASE(print_same_cwd)
{
  using boost::unit_test::framework::master_test_suite;
  const auto pth =  master_test_suite().argv[1];

  asio::io_context ctx;

  asio::readable_pipe rp{ctx};

  // default CWD
  bpv::process proc(ctx, pth, {"print-cwd"}, bpv::process_stdio{/*.in=*/{},/*.out=*/rp, /*.err*/{}});

  std::string out;
  bpv::error_code ec;

  auto sz = asio::read(rp, asio::dynamic_buffer(out),  ec);
  while (ec == asio::error::interrupted)
      sz += asio::read(rp, asio::dynamic_buffer(out),  ec);

  BOOST_CHECK(sz != 0);
  BOOST_CHECK_MESSAGE((ec == asio::error::broken_pipe) || (ec == asio::error::eof), ec.message());
  BOOST_CHECK_MESSAGE(bpv::filesystem::path(out) == bpv::filesystem::current_path(),
                     bpv::filesystem::path(out) << " != " << bpv::filesystem::current_path());

  proc.wait();
  BOOST_CHECK_MESSAGE(proc.exit_code() == 0, proc.exit_code());
}


BOOST_AUTO_TEST_CASE(popen)
{
    using boost::unit_test::framework::master_test_suite;
    const auto pth =  master_test_suite().argv[1];

    asio::io_context ctx;


    // default CWD
    bpv::popen proc(/*bpv::default_process_launcher(), */ctx, pth, {"echo"});

    BOOST_CHECK_EQUAL(asio::write(proc, asio::buffer("FOOBAR", 6)), 6u);

    proc.get_stdin().close();

    std::string res;
    boost::system::error_code ec;
    std::size_t n = asio::read(proc, asio::dynamic_buffer(res), ec);
    while (ec == asio::error::interrupted)
        n += asio::read(proc, asio::dynamic_buffer(res),  ec);

    BOOST_CHECK_MESSAGE(ec == asio::error::eof
                     || ec == asio::error::broken_pipe,
                     ec.message());
    BOOST_REQUIRE_GE(n, 1u);
    BOOST_CHECK_EQUAL(res, "FOOBAR");
    proc.get_stdin().close();
    proc.wait();
    BOOST_CHECK_MESSAGE(proc.exit_code() == 0, proc.exit_code());
}

BOOST_AUTO_TEST_CASE(print_other_cwd)
{
  using boost::unit_test::framework::master_test_suite;
  const auto pth = bpv::filesystem::absolute(master_test_suite().argv[1]);

  asio::io_context ctx;

  asio::readable_pipe rp{ctx};
  asio::writable_pipe wp{ctx};
  asio::connect_pipe(rp, wp);

  auto target = bpv::filesystem::canonical(bpv::filesystem::temp_directory_path());

  // default CWD
  bpv::process proc(ctx, pth, {"print-cwd"},
                    bpv::process_stdio{/*.in=*/{}, /*.out=*/wp, /*.err=*/{}},
                    bpv::process_start_dir(target));
  wp.close();

  std::string out;
  bpv::error_code ec;

  auto sz = asio::read(rp, asio::dynamic_buffer(out),  ec);
  while (ec == asio::error::interrupted)
    sz += asio::read(rp, asio::dynamic_buffer(out),  ec);

  BOOST_CHECK(sz != 0);
  BOOST_CHECK_MESSAGE((ec == asio::error::broken_pipe) || (ec == asio::error::eof), ec.message());


  if (out.back() != '/' && target.string().back() == '/')
      out += '/';

  BOOST_CHECK_MESSAGE(bpv::filesystem::path(out) == target,
                      bpv::filesystem::path(out) << " != " << target);

  proc.wait();
  BOOST_CHECK_MESSAGE(proc.exit_code() == 0, proc.exit_code() << " from " << proc.native_exit_code());
}


template<typename ... Inits>
std::string read_env(const char * name, Inits && ... inits)
{
  using boost::unit_test::framework::master_test_suite;
  const auto pth = bpv::filesystem::absolute(master_test_suite().argv[1]);


  asio::io_context ctx;

  asio::readable_pipe rp{ctx};
  asio::writable_pipe wp{ctx};
  asio::connect_pipe(rp, wp);

  bpv::process proc(ctx, pth, {"print-env", name}, bpv::process_stdio{/*.in-*/{}, /*.out*/{wp}, /*.err*/{}}, std::forward<Inits>(inits)...);

  wp.close();

  std::string out;
  bpv::error_code ec;

  auto sz = asio::read(rp, asio::dynamic_buffer(out),  ec);
  while (ec == asio::error::interrupted)
    sz += asio::read(rp, asio::dynamic_buffer(out),  ec);

  BOOST_CHECK_MESSAGE((ec == asio::error::broken_pipe) || (ec == asio::error::eof), ec.message());
  out.resize(sz);
  trim_end(out);
  printf("Read env (%ld) %s: '%s'\n", static_cast<long>(sz), name, out.c_str());

  proc.wait();
  BOOST_CHECK_EQUAL(proc.exit_code(), 0);

  return out;
}

BOOST_AUTO_TEST_CASE(environment)
{
  std::string path = ::getenv("PATH");
  BOOST_CHECK_EQUAL(read_env("PATH"), ::getenv("PATH"));

  auto c = bpv::environment::current();
  std::vector<bpv::environment::key_value_pair> sub_env(c.begin(), c.end());

  sub_env.push_back("FOOBAR=FOO-BAR");
  BOOST_CHECK_EQUAL("FOO-BAR", read_env("FOOBAR", bpv::process_environment{sub_env}));

  sub_env.push_back("XYZ=ZYX");
  auto itr = std::find_if(sub_env.begin(), sub_env.end(), [](const bpv::environment::key_value_pair & kv) {return kv.key() == bpv::environment::key("PATH");});
  path += static_cast<char>(bpv::environment::delimiter);
  path += "/bar/foo";
  bpv::environment::value pval = itr->value();
  pval.push_back("/bar/foo");
  *itr = bpv::environment::key_value_pair(bpv::environment::key("PATH"), pval);
  BOOST_CHECK_EQUAL(path, read_env("PATH", bpv::process_environment{sub_env}));

#if defined(BOOST_PROCESS_V2_WINDOWS)
  std::wstring wpath = L"PATh=" + std::wstring(_wgetenv(L"PatH"));
  BOOST_CHECK_EQUAL("FOO-BAR", read_env("FOOBAR", bpv::process_environment{L"FOOBAR=FOO-BAR", wpath.c_str()}));
  wpath += bpv::environment::delimiter;
  wpath += L"C:\\bar\\foo";
  BOOST_CHECK_EQUAL(
    bpv::detail::conv_string<char>(wpath.c_str() + 5, wpath.size() - 5)
    , read_env("pATH",   bpv::process_environment{wpath.c_str(), std::wstring(L"XYZ=ZYX")}));
#endif

  BOOST_CHECK_EQUAL(read_env("PATH", bpv::process_environment(bpv::environment::current())), ::getenv("PATH"));
}

BOOST_AUTO_TEST_CASE(exit_code_as_error)
{
  using boost::unit_test::framework::master_test_suite;
  const auto pth = bpv::filesystem::absolute(master_test_suite().argv[1]);

  asio::io_context ctx;

  bpv::process proc1(ctx, pth, {"exit-code", "0"});
  bpv::process proc2(ctx, pth, {"exit-code", "2"});
  bpv::process proc3(ctx, pth, {"sleep", "2000"});

  int called = 0;

  proc3.terminate();


  proc1.async_wait(
      [&](bpv::error_code ec, int)
      {
        called ++;
        bpv::check_exit_code(ec, proc1.native_exit_code());
        BOOST_CHECK(!ec);
      });

  proc2.async_wait(
      [&](bpv::error_code ec, int)
      {
        called ++;
        bpv::check_exit_code(ec, proc2.native_exit_code());
        BOOST_CHECK_MESSAGE(ec, ec.message());
      });

  proc3.async_wait(
      [&](bpv::error_code ec, int)
      {
        called ++;
        bpv::check_exit_code(ec, proc3.native_exit_code());
        BOOST_CHECK_MESSAGE(ec, ec.message());
      });


  ctx.run();
  BOOST_CHECK_EQUAL(called, 3);
}

BOOST_AUTO_TEST_CASE(bind_launcher)
{
  using boost::unit_test::framework::master_test_suite;
  const auto pth = bpv::filesystem::absolute(master_test_suite().argv[1]);

  asio::io_context ctx;

  asio::readable_pipe rp{ctx};

  auto target = bpv::filesystem::canonical(bpv::filesystem::temp_directory_path());

  auto l = bpv::bind_default_launcher(bpv::process_start_dir(target));
  std::vector<std::string> args = {"print-cwd"};
  // default CWD
  bpv::process proc = l(ctx, pth, args, bpv::process_stdio{/*.in=*/{}, /*.out=*/rp, /*.err=*/{}});

  std::string out;
  bpv::error_code ec;

  auto sz = asio::read(rp, asio::dynamic_buffer(out),  ec);
  while (ec == asio::error::interrupted)
    sz += asio::read(rp, asio::dynamic_buffer(out),  ec);

  BOOST_CHECK_MESSAGE((ec == asio::error::broken_pipe) || (ec == asio::error::eof), ec.message());
  BOOST_REQUIRE(sz != 0);

  if (out.back() != '/' && target.string().back() == '/')
      out += '/';

  BOOST_CHECK_MESSAGE(bpv::filesystem::path(out) == target,
                      bpv::filesystem::path(out) << " != " << target);

  proc.wait();
  BOOST_CHECK_MESSAGE(proc.exit_code() == 0, proc.exit_code() << " from " << proc.native_exit_code());
}

BOOST_AUTO_TEST_CASE(async_interrupt)
{
    if (!can_interrupt)
      return;

    asio::io_context ctx;
    using boost::unit_test::framework::master_test_suite;
    const auto pth = bpv::filesystem::absolute(master_test_suite().argv[1]);


    bpv::process proc(ctx, pth, {"sigint"}
#if defined(BOOST_PROCESS_V2_WINDOWS)
   , bpv::windows::create_new_process_group
#endif
    );

    asio::steady_timer tim{ctx, std::chrono::milliseconds(200)};
    asio::cancellation_signal sig;

    bpv::async_execute(std::move(proc),
                       asio::bind_cancellation_slot(
                            sig.slot(),
                            [](boost::system::error_code ec, int res)
                            {
                              BOOST_CHECK(!ec);
                              BOOST_CHECK_EQUAL(
                                  bpv::evaluate_exit_code(res) & ~SIGTERM, 0);
                            }));

    tim.async_wait([&](bpv::error_code) { sig.emit(asio::cancellation_type::total); });
    ctx.run();
}

BOOST_AUTO_TEST_CASE(async_request_exit)
{
    asio::io_context ctx;
    using boost::unit_test::framework::master_test_suite;
    const auto pth = bpv::filesystem::absolute(master_test_suite().argv[1]);

    bpv::process proc(ctx, pth, {"sigterm"}
#if defined(BOOST_PROCESS_V2_WINDOWS)
      , bpv::windows::show_window_minimized_not_active
      , bpv::windows::create_new_console
#endif
    );

    asio::steady_timer tim{ctx, std::chrono::milliseconds(250)};
    asio::cancellation_signal sig;

    bpv::async_execute(std::move(proc),
        asio::bind_cancellation_slot(
            sig.slot(),
            [](boost::system::error_code ec, int res)
            {
              BOOST_CHECK(!ec);
              BOOST_CHECK_EQUAL(bpv::evaluate_exit_code(res) & ~SIGTERM, 0);
            }));

    tim.async_wait([&](bpv::error_code) { sig.emit(asio::cancellation_type::partial); });
    ctx.run();
}

BOOST_AUTO_TEST_CASE(async_cancel_wait)
{
  asio::io_context ctx;
  using boost::unit_test::framework::master_test_suite;
  const auto pth = bpv::filesystem::absolute(master_test_suite().argv[1]);

  bpv::process proc(ctx, pth, {"sleep", "1000"});

  asio::steady_timer tim{ctx, std::chrono::milliseconds(250)};
  asio::cancellation_signal sig;

  // check that the async_wait gets properly cancelled
  proc.async_wait(asio::cancel_after(std::chrono::milliseconds(100),
                         [&](boost::system::error_code ec, int)
                         {
                           BOOST_CHECK_EQUAL(ec, asio::error::operation_aborted);
                           BOOST_CHECK(proc.running());
                           if (proc.running())
                             proc.terminate();
                         }));

  ctx.run();
}

#if defined(BOOST_POSIX_API)

struct capture_pid
{
  pid_t &pid;
  template<typename Launcher>
  void on_error(Launcher &launcher, const bpv::filesystem::path& executable,
                const char * const * (&/*cmd_line*/), const bpv::error_code & ec)
  {
    BOOST_REQUIRE(!bpv::filesystem::exists(executable));
    this->pid = launcher.pid;
  }
};

BOOST_AUTO_TEST_CASE(no_zombie)
{
  asio::io_context ctx;
  using boost::unit_test::framework::master_test_suite;
  const auto pth = bpv::filesystem::absolute(master_test_suite().argv[1]);

  pid_t res{-1};


  boost::system::error_code ec;
  bpv::default_process_launcher()(ctx, ec, "/send/more/cops", std::vector<std::string>{}, capture_pid{res});
  BOOST_CHECK(ec == boost::system::errc::no_such_file_or_directory);

  BOOST_REQUIRE(res != -1);
  BOOST_CHECK(res != 0);
  auto r = waitpid(res, nullptr, 0);
  BOOST_CHECK(r < 0);
  BOOST_CHECK_EQUAL(errno, ECHILD);
}

BOOST_AUTO_TEST_CASE(async_terminate_code)
{
  asio::io_context ctx;
  using boost::unit_test::framework::master_test_suite;
  const auto pth = bpv::filesystem::absolute(master_test_suite().argv[1]);

  bpv::process proc(ctx, pth, {"sleep", "1000"});

  proc.async_wait([&](boost::system::error_code ec, int code)
                  {
                    BOOST_CHECK_MESSAGE(!ec, ec.what());
                    BOOST_CHECK_EQUAL(code, SIGKILL);
                    BOOST_CHECK(!proc.running());
                 });

  asio::post(ctx, [&]{proc.terminate();});

  ctx.run();
}

#endif


BOOST_AUTO_TEST_SUITE_END();

