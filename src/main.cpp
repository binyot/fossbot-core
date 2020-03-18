#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <queue>
#include <map>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <algorithm>

#include <spdlog/spdlog.h>
#include <docopt/docopt.h>

#include "rfcomm.h"
#include "servo.h"
#include "parse.h"
#include "concurrent_queue.h"

#define FOSSBOT_CORE_IN_STDIN 1

static constexpr auto USAGE =
  R"(FOSSBot Core.

   Usage:
         core [-v | -vv | -vvv] [--servo-file FILE] [--channel CHANNEL]

 Options:
         --version            Show version.
         -h --help            Show this screen.
         -v --verbose         Set verbosity level [default: 0]
         --servo-file=FILE    Set servo control character file [default: /dev/custom_leds]
         --channel=CHANNEL    Specify Bluetooth channel [default: 3]
)";

auto parse_args(int argc, char **argv) -> std::tuple<uint8_t, std::string>;
auto set_verbosity(long level) -> void;

template<typename Q>
auto network_worker(uint8_t channel, Q &queue, std::atomic<bool> &done) -> void;

auto main(int argc, char **argv) -> int
{
  const auto [channel, servo_path] = parse_args(argc, argv);

  const auto motion_time_comp = [](const core::servo_motion &l, const core::servo_motion &r) { return l.time > r.time; };
  auto done = std::atomic<bool>{false};
  auto motion_queue = nonstd::concurrent_queue<core::servo_motion, std::priority_queue<core::servo_motion, std::vector<core::servo_motion>, decltype(motion_time_comp)>>(motion_time_comp);
  auto network_thread = std::thread(network_worker<decltype(motion_queue)>, channel, std::ref(motion_queue), std::ref(done));

  auto servo_file = std::ofstream(servo_path, std::ios::out | std::ios::binary);
  while (!done.load()) {
    const auto motion = motion_queue.wait_pop(); // could be too slow
    std::this_thread::sleep_until(motion.time);
    core::write_change(servo_file, motion);
  }

  network_thread.join();
}

template<typename Q>
auto network_worker([[maybe_unused]] uint8_t channel, Q &queue, std::atomic<bool> &done) -> void
{
  const auto push_program = [&queue](const std::string &program) {
    const auto statements = core::parse_program(program);
    auto motions = std::vector<core::servo_motion>{};
    motions.reserve(statements.size());
    const auto time = core::clock_t::now();
    for (const auto &statement : statements) {
      motions.push_back({
        core::to_motion(statement, time)
      });
    }
    // could be too slow if locking each time
    for (const auto &motion : motions) {
      queue.push(motion);
    }
    spdlog::debug("Pushed {} motions", motions.size());
  };

  const auto handle = [&done, &push_program](std::istream &is, std::ostream &os) {
    // TODO: make proper protocol
    auto strbuf = std::string{};
    for (std::string line; std::getline(is, line) or !done.load();) {
      if (line == "push") {
        spdlog::debug("Buffer with size {} pushed", strbuf.size());
        push_program(strbuf);
        os << "pushed\n";
        os.flush();
        strbuf.clear();
      } else {
        strbuf.append(line + "\n");
      }
    }
  };
#ifdef FOSSBOT_CORE_IN_STDIN
  core::listen_to_stdin(handle);
#else
  core::listen_to_rfcomm(handle, channel);
#endif
}

auto parse_args(int argc, char **argv) -> std::tuple<uint8_t, std::string>
{
  auto args = docopt::docopt(
    USAGE,
    { std::next(argv), std::next(argv, argc) },
    true,
    "FOSSBot Core 0.1");

  set_verbosity(args["--verbose"].asLong());
  const auto channel = static_cast<uint8_t>(args["--channel"].asLong());
  spdlog::debug("Using channel {}", channel);
  const auto servo_file = args["--servo-file"].asString();
  spdlog::debug("Using servo control file '{}'", servo_file);
  return std::make_tuple(channel, servo_file);
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