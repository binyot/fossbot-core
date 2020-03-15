#ifndef FOSSBOT_CORE_DEFS_H
#define FOSSBOT_CORE_DEFS_H

#include <chrono>
#include <utility>
#include <map>

namespace core {
using clock_t = std::chrono::steady_clock;
using time_point_t = std::chrono::time_point<clock_t>;
using duration_t = std::chrono::milliseconds;
using addr_t = uint8_t;

template<typename T>
using range_t = std::pair<T, T>;

static const std::map<std::string, addr_t> servo_addr_map{
  { "l_hand", 0x00 },
  { "r_hand", 0x01 },
  { "l_foot", 0x02 },
  { "r_foot", 0x03 },
  { "neck", 0x04 },
  { "l_shld", 0x05 },
  { "r_shld", 0x06 },
  { "l_ankl", 0x07 },
  { "r_ankl", 0x08 },
  { "l_knee", 0x09 },
  { "r_knee", 0x0A },
  { "l_thig", 0x0B },
  { "r_thig", 0x0C },
  { "l_leg", 0x0D },
  { "r_leg", 0x0E },
};
}// namespace core

#endif//FOSSBOT_CORE_DEFS_H
