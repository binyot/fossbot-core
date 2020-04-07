#ifndef FOSSBOT_CORE_PARSE_H
#define FOSSBOT_CORE_PARSE_H

#include <vector>
#include <exception>
#include <string>

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

template<typename C>
auto serialize_program(const C &statements) -> std::string
{
  auto body = std::string{};
  auto sorted_statements(statements);
  std::sort(
    begin(sorted_statements), end(sorted_statements), [](const auto &l, const auto &r) {
           return l.time_offset < r.time_offset;
    });
  auto old_time = sorted_statements.front().time_offset;
  body += fmt::format("{}:\n", old_time.count());
  for (const auto &statement : sorted_statements) {
    if (const auto new_time = statement.time_offset; new_time != old_time) {
      body += fmt::format("{}:\n", new_time.count());
      old_time = new_time;
    }
    body += fmt::format("\t{} -> {}\n", statement.name, statement.angle);
  }
  return body;
}

}// namespace core::parse



#endif//FOSSBOT_CORE_PARSE_H
