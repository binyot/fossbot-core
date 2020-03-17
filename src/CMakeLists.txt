find_package(Threads)

add_executable(core main.cpp servo.h defs.h servo.cpp rfcomm.h rfcomm.cpp parse.h parse.cpp concurrent_queue.h)

target_link_libraries(
  core
  PRIVATE project_options
          project_warnings
          CONAN_PKG::docopt.cpp
          CONAN_PKG::fmt
          CONAN_PKG::spdlog
          Threads::Threads)
