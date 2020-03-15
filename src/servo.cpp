#include <spdlog/spdlog.h>

#include "servo.h"

namespace core {

auto write_change(std::ostream &os, const servo_motion &motion) -> void
{
  os << motion.addr << motion.angle;
  os.flush();
  spdlog::debug("Writing {:#04x} to servo {:#04x}", motion.angle, motion.addr);
}

}// namespace core