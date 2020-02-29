# Generic test that uses conan libs
add_executable(core main.cpp)
target_link_libraries(
  core PRIVATE project_options project_warnings CONAN_PKG::docopt.cpp
               CONAN_PKG::fmt CONAN_PKG::spdlog)
