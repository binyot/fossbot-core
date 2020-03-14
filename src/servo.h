#ifndef FOSSBOT_CORE_SERVO_H
#define FOSSBOT_CORE_SERVO_H

#include <cstdint>
#include <memory>
#include <ostream>

#include "defs.h"

namespace core {

struct servo_motion
{
  uint8_t addr;
  uint8_t angle;
  time_point_t time;
};

auto write_change(std::ostream &os, const servo_motion &motion) -> void;

}// namespace core

#endif//FOSSBOT_CORE_SERVO_H
