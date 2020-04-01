#include "parsers.h"

std::string NextRawArgument(std::string& raw_arguments) {
  std::string argument = "";
  const auto separator = raw_arguments.find(consts::argument_separator);
  if (separator == std::string::npos) {
    if(raw_arguments.empty())
      assert(false && "No more arguments left");
    else {
      argument = raw_arguments;
      raw_arguments.clear();
    }
  } else {
    argument = raw_arguments.substr(0, separator);
    raw_arguments = raw_arguments.substr(separator + 1 /*ignore separator*/);
  }
  return argument;
}

// full specialisation in cpp
template<>
int UnificatedArguments::NextArgument() {
  return std::stoi(NextRawArgument(raw_arguments_));
}
template<>
double UnificatedArguments::NextArgument() {
  return std::stod(NextRawArgument(raw_arguments_));
}
template<>
std::string UnificatedArguments::NextArgument() {
  return NextRawArgument(raw_arguments_);
}

NameAndParams::NameAndParams(std::string input)
  : func_name_(NextRawArgument(input)), func_args_(input) {
}
