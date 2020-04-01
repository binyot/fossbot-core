#include <iostream>
#include <fstream>
#include <string>
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
#include "concurrent_priority_queue.h"
#include "util.h"

#include "cyboto/core.h"

#define FOSSBOT_CORE_IN_STDIN 1

static constexpr auto USAGE =
  R"(FOSSBot Core.

   Usage:
         core [-v | -vv | -vvv] [--servo-file FILE] [--init INIT] [--channel CHANNEL]

 Options:
         --version            Show version.
         -h --help            Show this screen.
         -v --verbose         Set verbosity level [default: 0]
         --servo-file=FILE    Set servo control character file [default: /dev/custom_leds]
         --init=INIT          Set file with initial commands path [default: init]
         --channel=CHANNEL    Specify Bluetooth channel [default: 3]
)";

auto parse_args(int argc, char **argv) -> std::tuple<uint8_t, std::string, std::string>;
auto set_verbosity(long level) -> void;

template<typename Q>
auto network_worker(uint8_t channel, Q &queue, std::atomic<bool> &done, std::string init_file) -> void;

auto main(int argc, char **argv) -> int
{
  const auto [channel, servo_file_path, init_file] = parse_args(argc, argv);

  const auto motion_time_comp = [](const core::servo_motion &l, const core::servo_motion &r) { return l.time > r.time; };
  auto done = std::atomic<bool>{ false };
  auto motion_queue = nonstd::concurrent_priority_queue<core::servo_motion, std::vector<core::servo_motion>, decltype(motion_time_comp)>(motion_time_comp);
  auto network_thread = std::thread(network_worker<decltype(motion_queue)>, channel, std::ref(motion_queue), std::ref(done), init_file);

  auto servo_file = std::ofstream(servo_file_path, std::ios::out | std::ios::binary);
  while (!done.load()) {
    // Check closest time;
    // wait until closest time or the next queue push;
    // pop and write when time is right
    const auto closest_time = motion_queue.wait_top().time;
    const auto next_motion = motion_queue.wait_new_top_for(closest_time - core::clock_t::now());
    if (const auto motion = next_motion.value(); motion.time <= core::clock_t::now()) {
      motion_queue.pop();
      core::write_change(servo_file, motion);
    }
  }

  network_thread.join();
}

template<typename Q>
auto network_worker([[maybe_unused]] uint8_t channel, Q &queue, std::atomic<bool> &done, [[maybe_unused]] std::string init_file) -> void
{
  auto &functionCore = Core::getCore();
  functionCore.CallFunction("ExampleWithTailAndDelay@10000");

  auto command_map = std::map<std::string, std::vector<core::motion_statement>>{};

  const auto push_program = [&](const std::vector<core::motion_statement> &statements) {
    spdlog::debug("Converting {} statements", statements.size());
    auto motions = std::vector<core::servo_motion>{};
    motions.reserve(statements.size());
    const auto time = core::clock_t::now();
    for (const auto &statement : statements) {
      motions.push_back({ core::to_motion(statement, time) });
    }
    // could be too slow if locking each time
    for (const auto &motion : motions) {
      queue.push(motion);
    }
    spdlog::debug("Pushed {} motions", motions.size());
  };

  const auto handle = [&](std::istream &is, [[maybe_unused]] std::ostream &os) {
    // TODO: make proper protocol
    // pleeeease rewrite this
    enum class state_t {
      out,
      create_command,
      run_command,
      command_body,
    };
    auto state = state_t::out;
    auto buffer = std::string{};
    auto command_name = std::string{};
    for (std::string line; std::getline(is, line) and !done.load();) {
      switch (state) {
      case state_t::out:
        if (const auto word = nonstd::trim(line); word == "create_command") {
          state = state_t::create_command;
        } else if (word == "run_command") {
          state = state_t::run_command;
        } else if (word == "list_commands") {
          for (const auto &[name, body] : command_map) {
            os << name << std::endl;
          }
        }
        break;

      case state_t::create_command:
        if (const auto name = nonstd::trim(line); name.length() != 0) {
          command_name = name;
          if (const auto it = command_map.find(name); it != end(command_map)) {
            spdlog::warn("Command {} will be overridden", name);
          }
          state = state_t::command_body;
        } else {
          spdlog::error("Invalid command name");
          state = state_t::out;
        }
        break;

      case state_t::run_command:
        if (const auto name = nonstd::trim(line); command_map.find(name) != end(command_map)) {
          const auto &statements = command_map[name];
          spdlog::debug("Pushing program {} with {} statements", name, statements.size());
          push_program(statements);
        } else {
          spdlog::error("Command {} does not exist", name);
        }
        state = state_t::out;
        break;

      case state_t::command_body:
        if (nonstd::trim(line) == "end") {
          command_map[command_name] = core::parse_program(buffer);
          buffer.clear();
          state = state_t::out;
          spdlog::info("Command {} created", command_name);
        } else {
          buffer.append(line + "\n");
        }
        break;
      }
    }
  };

  // input from file first
  auto infile = std::ifstream(init_file);
  auto sink = std::ofstream{};
  handle(infile, sink);

  // input from external source
#ifdef FOSSBOT_CORE_IN_STDIN
  core::listen_to_stdin(handle);
#else
  core::listen_to_rfcomm(handle, channel);
#endif
}

auto parse_args(int argc, char **argv) -> std::tuple<uint8_t, std::string, std::string>
{
  auto args = docopt::docopt(
    USAGE,
    { std::next(argv), std::next(argv, argc) },
    true,
    "FOSSBot Core 0.1");

  set_verbosity(args["--verbose"].asLong());
  const auto channel = static_cast<uint8_t>(args["--channel"].asLong());
  spdlog::debug("Using channel {}", channel);
  const auto servo_file_path = args["--servo-file"].asString();
  spdlog::debug("Using servo control file '{}'", servo_file_path);
  const auto init_file = args["--init"].asString();
  spdlog::debug("Using init file '{}'", servo_file_path);
  return std::make_tuple(channel, servo_file_path, init_file);
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
