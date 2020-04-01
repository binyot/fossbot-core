#ifndef BASICTRIGGER_H
#define BASICTRIGGER_H

#include "parsers.h"

/// base, should not be used
class BasicTrigger {
 public:
  BasicTrigger();
  bool Triggered() const { return state_; }
 protected:
  bool state_ = false;
  int time_since_triggered_ = 0; // or we need trigger time?
};

// TODO think about better name
/// in contrast to other triggers do not catch condition
/// changes, but asks for needed values itself.
class StaticTrigger : public BasicTrigger {
 public:
  StaticTrigger() : BasicTrigger() { state_ = true; } // TODO implement later
};

/// counts calls of function or some parameters, also used as a timer
template<typename T>
class CountTrigger : public BasicTrigger {
 public:
  CountTrigger();
 private:
  T value_;
};

/// compares other triggers
template<typename T>
class ComparisonTrigger : public BasicTrigger {
 public:
  // TODO use it as lambda
  enum class ComparisonType {
    More,
    MoreOrEqual,
    Less,
    LessOrEqual,
    Equal
  };

  ComparisonTrigger();
 private:
  // TODO add max time between calls
  ComparisonType comparison_type_;
  T compared_value_;
};

class ComplexTrigger : public BasicTrigger {
 public:
  enum class OperationType {
    And,
    Or
  };

  ComplexTrigger();
 private:
  OperationType operation_type_;
};

/// can call functions
class FunctionTrigger : public BasicTrigger {
 public:
  FunctionTrigger();
 private:
  const BasicTrigger* trigger;
};

#endif // BASICTRIGGER_H
