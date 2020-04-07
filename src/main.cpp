#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <map>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <cerrno>

#include <spdlog/spdlog.h>
#include <docopt/docopt.h>
#include <nlohmann/json.hpp>

#include "network.h"
#include "servo.h"
#include "parse.h"
#include "concurrent_priority_queue.h"
#include "util.h"

//#define FOSSBOT_CORE_IN_STDIN 1

static constexpr auto USAGE =
  R"(FOSSBot Core.

   Usage:
         core [-v | -vv | -vvv] [--servo-file FILE] [--init INIT] [--tcp] [--channel CHANNEL]

 Options:
         --version            Show version.
         -h --help            Show this screen.
         -v --verbose         Set verbosity level [default: 0]
         --servo-file=FILE    Set servo control character file [default: /dev/custom_leds]
         --init=INIT          Set file with initial commands path [default: init]
         -t --tcp             Use TCP/IP instead of Bluetooth RFCOMM [default: false]
         --channel=CHANNEL    Specify Bluetooth channel (or TCP port) [default: 1]
)";

auto parse_args(int argc, char **argv) -> std::tuple<int, bool, std::string, std::string>;
auto set_verbosity(long level) -> void;

template<typename Q>
auto network_worker(uint8_t channel, bool use_tcp, Q &queue, std::atomic<bool> &done, std::string init_file) -> void;

auto main(int argc, char **argv) -> int
{
  const auto [channel, use_tcp, servo_path, init_file] = parse_args(argc, argv);

  const auto motion_time_comp = [](const core::servo_motion &l, const core::servo_motion &r) { return l.time > r.time; };
  auto done = std::atomic<bool>{ false };
  auto motion_queue = nonstd::concurrent_priority_queue<core::servo_motion, std::vector<core::servo_motion>, decltype(motion_time_comp)>(motion_time_comp);
  auto network_thread = std::thread(network_worker<decltype(motion_queue)>, channel, use_tcp, std::ref(motion_queue), std::ref(done), init_file);

  auto servo_file = std::ofstream(servo_path, std::ios::out | std::ios::binary);
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
auto network_worker([[maybe_unused]] uint8_t channel, bool use_tcp, Q &queue, std::atomic<bool> &done, [[maybe_unused]] std::string init_file) -> void
{
  auto command_map = std::map<std::string, std::vector<core::motion_statement>>{};

  const auto push_program = [&queue](const std::vector<core::motion_statement> &statements) {
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

  const auto handle = [&](std::istream &is, std::ostream &os) {
    enum class request_type {
      list,
      run,
      create,
      null
    };
    const auto request_type_map = std::map<std::string, request_type> {
      { "list", request_type::list },
      { "run", request_type::run },
      { "create", request_type::create },
    };

    for (std::string line; std::getline(is, line) and !done.load();) {
      try {
        const auto request = nlohmann::json::parse(line);
        const std::string type_name = request.at("type");
        const auto type = nonstd::at_or(request_type_map, type_name, request_type::null);
        spdlog::info("Received {} request", type_name);
        switch (type) {
        case request_type::list: {
          auto names = std::vector<std::string_view>{};
          names.reserve(command_map.size());
          for (const auto &kv : command_map) {
            names.push_back(kv.first);
          }
          auto response = nlohmann::json{
            {"type", type_name},
            {"names", names},
          };
          spdlog::debug("Sending {}", response.dump());
          os << response << std::endl;
          break;
        }
        case request_type::create: {
          const std::string name = request.at("name");
          const std::string body = request.at("body");
          const auto statements = core::parse_program(body);
          command_map[name] = statements;
          break;
        }
        case request_type::run: {
          const std::string name = request.at("name");
          if (const auto kv = command_map.find(name); kv != end(command_map)) {
            const auto statements = command_map.at(name);
            push_program(statements);
          } else {
            spdlog::error("Unknown command \"{}\"", name);
          }
          break;
        }
        default:
          spdlog::error("Unknown request type \"{}\" received", type_name);
          break;
        }
      } catch (...) {
        spdlog::error("Ill-formed request received");
      } //hmmm
      spdlog::debug("Request: {}", line);
    }
  };

  // input from file first
//  auto infile = std::ifstream(init_file);
//  auto sink = std::ofstream{};
//  handle(infile, sink);

  // input from external source
#ifdef FOSSBOT_CORE_IN_STDIN
  core::listen_to_stdin(handle);
#else
  if (use_tcp) {
    core::listen_to_tcp(handle, channel);
  } else {
    core::listen_to_rfcomm(handle, channel);
  }
#endif
}

auto parse_args(int argc, char **argv) -> std::tuple<int, bool, std::string, std::string>
{
  auto args = docopt::docopt(
    USAGE,
    { std::next(argv), std::next(argv, argc) },
    true,
    "FOSSBot Core 0.1");

  set_verbosity(args["--verbose"].asLong());
  const auto channel = static_cast<uint8_t>(args["--channel"].asLong());
  spdlog::debug("Using channel {}", channel);
  const auto use_tcp = args["--tcp"].asBool();
  spdlog::debug("Using {} with {} {}",
    use_tcp ? "TCP/IP" : "Bluetooth RFCOMM",
    use_tcp ? "port" : "channel",
    channel
    );
  const auto servo_file = args["--servo-file"].asString();
  spdlog::debug("Using servo control file '{}'", servo_file);
  const auto init_file = args["--init"].asString();
  spdlog::debug("Using init file '{}'", init_file);
  return std::make_tuple(channel, use_tcp, servo_file, init_file);
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
