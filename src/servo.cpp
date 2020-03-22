#include <spdlog/spdlog.h>

#include "servo.h"

namespace core {

auto write_change(std::ostream &os, const servo_motion &motion) -> void
{
  os << motion.addr << motion.angle;
  os.flush();
  spdlog::debug("Writing {:#04x} to servo {:#04x}", motion.angle, motion.addr);
}

auto operator==(const servo_motion &l, const servo_motion &r) -> bool
{
  return (l.addr == r.addr) && (l.time == r.time) && (l.angle == r.angle);
}

}// namespace core