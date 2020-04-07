find_package(Threads)

add_executable(core main.cpp servo.h defs.h servo.cpp network.h network.cpp parse.h parse.cpp)

target_link_libraries(
  core
  PRIVATE project_options
          project_warnings
          CONAN_PKG::docopt.cpp
          CONAN_PKG::fmt
          CONAN_PKG::spdlog
          CONAN_PKG::nlohmann_json
          Threads::Threads)
