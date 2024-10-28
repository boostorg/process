//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/process/v2/experimental/stream.hpp>

#include <boost/algorithm/string/case_conv.hpp>

#include <boost/asio/coroutine.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/yield.hpp>


namespace net = boost::asio;
namespace bp = boost::process::v2;

char buf[512];

bool async_check = false;

struct async_op : net::coroutine
{
  bp::experimental::basic_stream<net::io_context::executor_type> &in, &out;

  void operator()(bp::error_code ec = {}, std::size_t n = {})
  {
    if (ec)
      return;
    reenter(this)
    {
      while (in.is_open())
      {
        async_check = false;
        net::post(in.get_executor(), []{async_check=true;});
        yield in.async_read_some(net::buffer(buf), *this);
        assert(async_check);
        boost::algorithm::to_upper(buf);
        yield net::async_write(out, net::buffer(buf, n), *this);
      }
    }
  }
};

int main(int argc, char * argv[])
{
  net::io_context ctx;

  auto in = bp::experimental::open_stdin(ctx.get_executor());
  assert(in.is_pty());
  assert(in.echo());
  assert(in.line());

  std::vector<std::string> args{argv + 1, argv + argc};

  const bool async = std::find(args.begin(), args.end(), "--async")  != args.end();
  const bool stder = std::find(args.begin(), args.end(), "--stderr") != args.end();
  const bool echo  = std::find(args.begin(), args.end(), "--echo")   != args.end();
  const bool line  = std::find(args.begin(), args.end(), "--line")   != args.end();
  const bool wait  = std::find(args.begin(), args.end(), "--wait-resize")   != args.end();

  auto out = stder ? bp::experimental::open_stderr(ctx.get_executor())
                   : bp::experimental::open_stdout(ctx.get_executor());


  in.set_echo(echo);
  in.set_line(line);

  assert(in.echo() == echo);
  assert(in.line() == line);

  net::write(out, net::buffer("sync", 4));

  if (wait)
  {

    in.async_wait_for_size_change(
        [&](bp::error_code ec, bp::experimental::console_size_t cs)
        {
          assert(!ec);
          printf("cols=%d rows=%d\n", cs.columns, cs.rows);
        });

    net::post(ctx, [&]{
      const auto sz = in.console_size();
      printf("cols=%d rows=%d\n", sz.columns, sz.rows);
    });
    ctx.run();
  }
  else if (async)
  {
    net::post(ctx, async_op{{}, in, out});
    ctx.run();
  }
  else
  {
    char buf[512];
    auto f = fopen("./test.txt", "w");
    while (in.is_open())
    {
      auto n = in.read_some(net::buffer(buf));
      boost::algorithm::to_upper(buf);
      fwrite(buf, 1u, n, f);
      fflush(f);
      net::write(out, net::buffer(buf, n));
    }
  }

  return 0u;
}