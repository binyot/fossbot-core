#ifndef COMMON_H
#define COMMON_H

#include <limits>
#include <utility>
#include <assert.h>
#include <string>
#include <type_traits>
#include <iostream>

#include "consts.h"

template<class... Args>
std::string ToFuncArgs(const Args... args) {
  std::string result = ((to_string_custom(args) + consts::argument_separator) + ...);
  result.pop_back(); // remove last @
  // std::cout << result << std::endl;
  return result;
}

namespace geom { //TODO bad name

  template<typename ValueType>
  bool IsInRange(ValueType value, std::pair<ValueType, ValueType> bounds) {
    assert(bounds.first < bounds.second
           && "second value must be bigger or equal to the first one");
    return (value >= bounds.first) &&
           (value <= bounds.second);
  }
}

#endif // COMMON_H
