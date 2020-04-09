#ifndef FOSSBOT_CORE_SERVO_H
#define FOSSBOT_CORE_SERVO_H

#include <cstdint>
#include <memory>
#include <ostream>

#include "defs.h"

namespace core {

/**
 * Represents a specific servo motion at and address at specific time to a specific angle
 */
struct servo_motion
{
  uint8_t addr;
  uint8_t angle;
  time_point_t time;
};

auto operator==(const servo_motion &, const servo_motion &) -> bool;
auto operator!=(const servo_motion &, const servo_motion &) -> bool;

/**
 * Apply servo motion using output stream (e.g. character device).
 * Only address and angle are used, change is applied instantly,
 * omitting motion time point.
 * @param os
 * @param motion
 */
auto write_change(std::ostream &os, const servo_motion &motion) -> void;

}// namespace core

#endif//FOSSBOT_CORE_SERVO_H
