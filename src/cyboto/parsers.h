#ifndef FUNCTIONSIGNATURE_H
#define FUNCTIONSIGNATURE_H

#include <string>
#include <initializer_list>
#include <type_traits>
#include <iostream>

#include "consts.h"

class UnificatedArguments {
 public:
  UnificatedArguments(std::string raw_arguments)
    : raw_arguments_(raw_arguments) {}
 template<typename T>
 T NextArgument();
 std::string raw_arguments() { return raw_arguments_; }
 private:
  // separation by '@', this symbol not allowoded in any other cases
  std::string raw_arguments_;
};

class NameAndParams {
 public:
  NameAndParams(std::string input);
 protected:
  std::string Name() { return func_name_; }
  UnificatedArguments UnificatedArgs() { return func_args_; }
 private:
  std::string func_name_;
  UnificatedArguments func_args_;
};

class FunctionSignature : public NameAndParams{
 public:
  FunctionSignature(std::string input) : NameAndParams(input) {}
  std::string FunctionName() { return Name(); }
  UnificatedArguments Args() { return UnificatedArgs(); }
};

class EventSignature : public NameAndParams{
 public:
  using EventAndSender = std::pair<std::string, int>;
  EventSignature(std::string input, int sender_id = consts::id_not_needed)
    : NameAndParams(input), sender_id_(sender_id) {}
  EventAndSender GetEventAndSender() { return {Name(), sender_id_}; }
  template<typename T>
  T EventArg() { return UnificatedArgs().NextArgument<T>(); }
 private:
  int sender_id_;
};



template<typename T>
T NextArgument() {
  assert(false && "invalid return type");
}
template<>
double UnificatedArguments::NextArgument();
template<>
int UnificatedArguments::NextArgument();
template<>
std::string UnificatedArguments::NextArgument();

template<typename T>
std::string to_string_custom(const T& input) {
  if constexpr(std::is_arithmetic<T>::value) {
    return std::to_string(input);
  } else {
    return std::string(input);
  }
}

#endif // FUNCTIONSIGNATURE_H
