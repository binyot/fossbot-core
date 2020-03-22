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
using hw_angle_t = uint8_t;

template<typename T>
using range_t = std::pair<T, T>;

constexpr auto hw_angle_range = range_t<hw_angle_t>(0, 255);

struct servo_param
{
  addr_t addr;
  range_t<float> angle_range;
};

static const std::map<std::string, servo_param> servo_param_map{
  { "neck",
    { 0x00, { 0.f, 180.f } } },
  { "l_shld",
    { 0x01, { 0.f, 180.f } } },
  { "r_shld",
    { 0x02, { 0.f, 180.f } } },
  { "l_hand",
    { 0x03, { 0.f, 180.f } } },
  { "r_hand",
    { 0x04, { 0.f, 180.f } } },
  { "l_leg",
    { 0x05, { 0.f, 180.f } } },
  { "r_leg",
    { 0x06, { 0.f, 180.f } } },
  { "l_thig",
    { 0x07, { 0.f, 180.f } } },
  { "r_thig",
    { 0x08, { 0.f, 180.f } } },
  { "l_knee",
    { 0x09, { 0.f, 180.f } } },
  { "r_knee",
    { 0x0A, { 0.f, 180.f } } },
  { "l_ankl",
    { 0x0B, { 0.f, 180.f } } },
  { "r_ankl",
    { 0x0C, { 0.f, 180.f } } },
  { "l_foot",
    { 0x0D, { 0.f, 180.f } } },
  { "r_foot",
    { 0x0E, { 0.f, 180.f } } },
};

}// namespace core

#endif//FOSSBOT_CORE_DEFS_H
