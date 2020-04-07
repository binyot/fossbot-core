#ifndef FOSSBOT_CORE_PARSE_H
#define FOSSBOT_CORE_PARSE_H

#include <vector>
#include <exception>

#include "defs.h"
#include "servo.h"
#include "network.h"

namespace core {

struct syntax_error : public std::exception {};

struct motion_statement {
  std::string name;
  float angle;
  duration_t time_offset;
};

// TODO: change to lazy generator
auto parse_program(const std::string &program) -> std::vector<motion_statement>;
auto to_motion(const motion_statement &statement, time_point_t start_time) -> servo_motion;

}// namespace core::parse



#endif//FOSSBOT_CORE_PARSE_H
