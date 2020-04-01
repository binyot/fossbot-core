#ifndef CUSTOM_FUNCTION_H
#define CUSTOM_FUNCTION_H

#include "base_function.h"

#include <string>
#include <map>
#include <functional>

// here we add more complex functions.
// They must be inhereted from StandartFunction only for now.

class FunctionFactory {
 public:
  FunctionFactory();
  StandartFunction* GetFunction(FunctionSignature func_signature);
  StandartFunction* GetFunction(std::string name, UnificatedArguments args);
 private:
  ///6 arg: 1motor time, speed, 2motor time, speed, 3motor time, speed
  StandartFunction* ExampleRotate3Motors(UnificatedArguments args);
  StandartFunction* ExampleWithTailAndDelay(UnificatedArguments args);
  std::map<std::string,
           std::function<StandartFunction*(UnificatedArguments)>> available_functions_;
};

#endif // CUSTOM_FUNCTION_H
