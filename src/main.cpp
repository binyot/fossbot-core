#include <functional>
#include <iostream>

#include <spdlog/spdlog.h>


#include <docopt/docopt.h>

#include <iostream>

static constexpr auto USAGE =
  R"(FOSSBot Core.

    Usage:
          core (-h | --help)
          core --version
  Options:
          -h --help     Show this screen.
          --version     Show version.
)";

int main(int argc, const char **argv)
{
  std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
    { std::next(argv), std::next(argv, argc) },
    true,
    "FOSSBot 0.1");

  for (auto const &arg : args) {
    std::cout << arg.first << arg.second << std::endl;
  }


  //Use the default logger (stdout, multi-threaded, colored)
  spdlog::info("Hello, {}!", "World");

  fmt::print("Hello, from {}\n", "{fmt}");
}
