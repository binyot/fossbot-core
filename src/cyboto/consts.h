#ifndef CONSTS_H
#define CONSTS_H

#include <limits>
#include <string>
#include <vector>
#include <assert.h>
#include <chrono>

namespace consts {
  static constexpr double undefined_double = std::numeric_limits<double>::max();
  /// ms smallest unit of time, each base component func must durate that value.
  static constexpr int atomic_time_value = 100;
  static constexpr char function_manager_name[] = "function manager";
  static constexpr char fake_component[] = "FAKE_COMP";
  static constexpr char fake_function[]  = "FAKE_FUNC";
  static constexpr char function_finished[] = "fin";
  static constexpr char argument_separator[] = "@";
  static constexpr int id_not_needed = std::numeric_limits<int>::max();
};

namespace phys_comp {
  static constexpr char servo_m1[] = "servo_1";
  static constexpr char servo_m2[] = "servo_2";
  static constexpr char servo_m3[] = "servo_3";
}

#endif // CONSTS_H
