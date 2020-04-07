#include <algorithm>
#include <string>
#include <regex>
#include <tuple>

#include <spdlog/spdlog.h>

#include "defs.h"
#include "parse.h"

namespace core {

///
/// СТРАУСТРУП ДАЙ ПОДДЕРЖКУ string_view У regex
///

auto parse_program(const std::string &program) -> std::vector<motion_statement>
{
  spdlog::debug("Parsing program");
  std::vector<motion_statement> result;

  // matches time (1) and assignment list (2)
  static const std::regex expression_re{ R"(([0-9]+):\s*((?:\s*[a-zA-Z0-9_]+\s*->\s*(?:[-+]?[0-9]*\.?[0-9]+))*))" };

  for (auto expression = std::sregex_iterator{ begin(program), end(program), expression_re };
       expression != decltype(expression){};
       expression++) {
    const auto exp_base = cbegin(*expression);
    const auto time_base = exp_base + 1;
    const auto list_base = exp_base + 2;
    const auto time = std::stoull(time_base->str());
    const auto time_offset = std::chrono::milliseconds(time);
    const auto assignment_list = std::string(list_base->first, list_base->second);

    // matches assignment left (1) -> right (2)
    static const std::regex assignment_re{ R"(([a-zA-Z0-9_]+)\s*->\s*([-+]?[0-9]*\.?[0-9]+))" };
    for (auto assignment = std::sregex_iterator{ list_base->first, list_base->second, assignment_re };
         assignment != decltype(expression){};
         assignment++) {
      const auto ass_base = cbegin(*assignment);
      const auto name_base = ass_base + 1;
      const auto angle_base = ass_base + 2;
      const auto name = name_base->str();
      const auto angle = std::stof(angle_base->str());

      try {
        result.push_back({ name, angle, time_offset });
        spdlog::debug("Parsed {}: {} -> {}", time, name, angle);
      } catch (std::out_of_range &e) {
        throw syntax_error{};
      }
    }
  }

  return result;
}

auto to_motion(const motion_statement &statement, time_point_t start_time) -> servo_motion
{
  const auto params = servo_param_map.at(statement.name);
  const auto addr = params.addr;
  const auto angle_range = params.angle_range;
  const auto angle = std::clamp(statement.angle, angle_range.first, angle_range.second);
  const auto angle_span = angle_range.second - angle_range.first;
  const auto hw_angle_span = hw_angle_range.second - hw_angle_range.first;
  const auto hw_angle = static_cast<hw_angle_t>(angle / angle_span * hw_angle_span + hw_angle_range.first);
  const auto time = start_time + statement.time_offset;
  const auto motion = servo_motion{
    addr,
    hw_angle,
    time,
  };
  return motion;
}

}// namespace core
