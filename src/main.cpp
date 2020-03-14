#include <string>
#include <queue>
#include <thread>
#include <iostream>
#include <fstream>
#include <map>

#include <spdlog/spdlog.h>
#include <docopt/docopt.h>

#include "rfcomm.h"
#include "servo.h"

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

auto set_verbosity(long level) -> void;

auto main(int argc, const char **argv) -> int
{
  std::map<std::string, docopt::value> args = docopt::docopt(
    USAGE,
    { std::next(argv), std::next(argv, argc) },
    true,
    "FOSSBot Core 0.1");

  set_verbosity(args["--verbose"].asLong());
  auto channel = static_cast<uint8_t>(args["--channel"].asLong());
  auto rfcomm_handle = [](std::istream &is, std::ostream &os) -> bool {
    for (std::string line; std::getline(is, line);) {
      spdlog::info("In: {}", line);
      os << fmt::format("echo {}\n", line);
      os.flush();
    }
    return true;
  };
  auto network_thread = std::thread(core::listen_to_rfcomm, rfcomm_handle, channel);

  auto servo_file = std::ofstream("dev_custom_leds", std::ios::out | std::ios::binary);

  auto angle_change_comp = [](const core::servo_motion &l, const core::servo_motion &r) { return l.time > r.time; };
  std::priority_queue<core::servo_motion, std::vector<core::servo_motion>, decltype(angle_change_comp)>
    change_queue(angle_change_comp);

  auto now = core::clock_t::now();
  for (uint8_t i = 0; i < 10; i++) {
    change_queue.push(core::servo_motion{ i, static_cast<uint8_t>(i * 0x10 + 0x0F), now + std::chrono::seconds(i) });
  }

  while (!change_queue.empty()) {
    auto change = change_queue.top();
    change_queue.pop();
    std::this_thread::sleep_until(change.time);
    core::write_change(servo_file, change);
  }

  network_thread.join();
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