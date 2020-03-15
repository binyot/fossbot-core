#ifndef FOSSBOT_CORE_PARSE_H
#define FOSSBOT_CORE_PARSE_H

#include <variant>
#include <vector>
#include <exception>

#include "defs.h"
#include "servo.h"

namespace core {

struct syntax_error : public std::exception {};

struct motion_statement {
  addr_t addr;
  float angle;
  duration_t time_offset;
};

// TODO: change to lazy generator
auto parse_program(const std::string &program) -> std::vector<motion_statement>;

}// namespace core::parse


#endif//FOSSBOT_CORE_PARSE_H
