#include "function_factory.h"

#include "base_function.h"
#include "common.h"

FunctionFactory::FunctionFactory() {
  available_functions_.emplace("ExampleRotate3Motors",
                               [=](UnificatedArguments args)
                               { return this->ExampleRotate3Motors(args); });
  available_functions_.emplace("ExampleWithTailAndDelay",
                               [=](UnificatedArguments args)
                               { return this->ExampleWithTailAndDelay(args); });
}

StandartFunction* FunctionFactory::GetFunction(FunctionSignature func_signature) {
  return GetFunction(func_signature.FunctionName(), func_signature.Args());
}

StandartFunction* FunctionFactory::GetFunction(std::string name,
                                               UnificatedArguments args) {
  return available_functions_.at(name)(args);
}

StandartFunction* FunctionFactory::ExampleRotate3Motors(UnificatedArguments args) {
  StandartFunction* result = new StandartFunction();
  int m1_time = args.NextArgument<int>();
  double m1_speed = args.NextArgument<double>();
  int m2_time = args.NextArgument<int>();
  double m2_speed = args.NextArgument<double>();
  int m3_time = args.NextArgument<int>();
  double m3_speed = args.NextArgument<double>();
  result->AddBodyFunction<PhysicalFunction>(phys_comp::servo_m1,
                                            consts::atomic_time_value * m1_time,
                                            "Rotate", m1_speed);
  result->AddBodyFunction<PhysicalFunction>(phys_comp::servo_m2,
                                            consts::atomic_time_value * m2_time,
                                            "Rotate", m2_speed);
  result->AddBodyFunction<PhysicalFunction>(phys_comp::servo_m3,
                                            consts::atomic_time_value * m3_time,
                                            "Rotate", m3_speed);
  return result;
}

StandartFunction* FunctionFactory::ExampleWithTailAndDelay(UnificatedArguments args) {
  StandartFunction* result = new StandartFunction();
  int timer_time = args.NextArgument<int>();
  result->AddBodyFunction<PhysicalFunction>(phys_comp::servo_m1,
                                            1000 /*1 sec*/,
                                            "Rotate", 0.1);
  result->body_funcs_.insert(PhysicalFunction::Timer(timer_time, result));
  result->tail_funcs_.insert(ExampleRotate3Motors(ToFuncArgs(10, -0.03, 30, 0.03, 50, 0.02)));
  return result;
}
