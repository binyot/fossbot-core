#ifndef FOSSBOT_CORE_DEFS_H
#define FOSSBOT_CORE_DEFS_H

#include <chrono>
#include <utility>

namespace core {
  using clock_t = std::chrono::high_resolution_clock;
  using time_point_t = std::chrono::time_point<clock_t>;
  using addr_t = uint8_t;

  template<typename T>
  using range_t = std::pair<T, T>;
}

#endif//FOSSBOT_CORE_DEFS_H
