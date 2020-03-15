#include <algorithm>
#include <string>
#include <regex>
#include <cctype>
#include <iterator>
#include <tuple>

#include <spdlog/spdlog.h>
#include <iostream>

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
        const auto addr = servo_addr_map.at(name);
        result.push_back({addr, angle, time_offset});
      } catch (std::out_of_range &e) {
        throw syntax_error{};
      }
    }
  }

  return result;
}

}// namespace core
