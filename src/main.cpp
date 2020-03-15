#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <queue>
#include <map>
#include <thread>
#include <atomic>
#include <condition_variable>

#include <spdlog/spdlog.h>
#include <docopt/docopt.h>

#include "rfcomm.h"
#include "servo.h"
#include "parse.h"

static constexpr auto USAGE =
  R"(FOSSBot Core.

   Usage:
         core [-v | -vv | -vvv] [--channel CHANNEL]

 Options:
         --version            Show version.
         -h --help            Show this screen.
         -v --verbose         Set verbosity level [default: 0]
         --channel=CHANNEL    Specify Bluetooth channel [default: 3]
)";

auto parse_args(int argc, char **argv) -> std::tuple<uint8_t>;
auto set_verbosity(long level) -> void;

enum class net_state { start, program };

auto main(int argc, char **argv) -> int
{
  const auto [channel] = parse_args(argc, argv);

  auto rfcomm_handle = [](std::istream &is, std::ostream &os) -> bool {
    auto state = net_state::start;
    auto buffer = std::string{};
    for (std::string line; std::getline(is, line);) {
      spdlog::debug("In: {}", line);
      switch (state) {
      case net_state::start:
        if (line == "begin") {
          state = net_state::program;
        }
        break;
      case net_state::program:
          if (line == "end") {
            state = net_state::start;
            try {
              auto program = core::parse_program(buffer);
              spdlog::info("Program:");
              for (auto &s : program) {
                spdlog::info("{}: {:#04x} -> {}", s.time_offset.count(), s.addr, s.angle);
              }
            } catch (core::syntax_error &e) {
              spdlog::warn("Syntax error");
              os << "syntax error\n";
              os.flush();
            }
          }
          buffer.append(line + "\n");
        break;
      }
    }
    return true;
  };
  auto network_thread = std::thread(core::listen_to_rfcomm, rfcomm_handle, channel);
//
//  auto servo_file = std::ofstream("dev_custom_leds", std::ios::out | std::ios::binary);
//
//  auto angle_change_comp = [](const core::servo_motion &l, const core::servo_motion &r) { return l.time > r.time; };
//  std::priority_queue<core::servo_motion, std::vector<core::servo_motion>, decltype(angle_change_comp)>
//    change_queue(angle_change_comp);
//
//  auto now = core::clock_t::now();
//  for (uint8_t i = 0; i < 10; i++) {
//    change_queue.push(core::servo_motion{ i, static_cast<uint8_t>(i * 0x10 + 0x0F), now + std::chrono::seconds(i) });
//  }
//
//  while (!change_queue.empty()) {
//    auto change = change_queue.top();
//    change_queue.pop();
//    std::this_thread::sleep_until(change.time);
//    core::write_change(servo_file, change);
//  }

  network_thread.join();
}

//auto network_handle(uint8_t channel, std::queue<core::servo_motion> &queue, std::condition_variable &cv, std::mutex &m, std::atomic<bool> &done) -> void
//{
//  auto handle = [&](std::istream &is, std::ostream &os){
//    while(!done.load())
//    {
//      std::unique_lock lock(m);
//    }
//  };
//}

auto parse_args(int argc, char **argv) -> std::tuple<uint8_t>
{
  auto args = docopt::docopt(
    USAGE,
    { std::next(argv), std::next(argv, argc) },
    true,
    "FOSSBot Core 0.1");

  set_verbosity(args["--verbose"].asLong());
  const auto channel = static_cast<uint8_t>(args["--channel"].asLong());
  return std::make_tuple(channel);
}

auto set_verbosity(long level) -> void
{
  spdlog::level::level_enum log_level;
  switch (level) {
  case 1:
    log_level = spdlog::level::warn;
    break;
  case 2:
    log_level = spdlog::level::info;
    break;
  case 3:
    log_level = spdlog::level::debug;
    break;
  default:
    log_level = spdlog::level::err;
    break;
  }
  spdlog::set_level(log_level);
  spdlog::flush_on(log_level);
}