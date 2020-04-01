#ifndef BASEFUNCTION_H
#define BASEFUNCTION_H

#include <string>
#include <vector>
#include <set>
#include <memory>

#include "parsers.h"
#include "trigger.h"
#include "common.h"
enum class FunctionType {
  Base,
  Physical,
  Standart,
  Trigger
};

enum class FunctionStatus {
  NotActivated,
  BodyRunning,
  BodyCompleted,
  TailRunning,
  TailCompleted,
  Finished// all tail functions of that function recursevely completed
};

class FunctionBasement
{
 public:
  using Functions = std::set<FunctionBasement*>;

  using Events = std::vector<EventSignature>;
  FunctionBasement(FunctionType type, FunctionBasement* parent_ = nullptr);
  virtual void FunctionCalled();
  /// should be called befor adding to pool of active functions
  virtual bool check_state() { return true; }
  Events ReceiveEvents();
  bool need_to_delete() const { return need_to_delete_; }
  FunctionType type() const { return type_; }
  FunctionStatus status() const { return status_; }
  bool is_initialized() const {return is_initialized_; }
  virtual ~FunctionBasement();

protected:
  virtual void ChildFunctionStatusUpdated(FunctionBasement *child_func);
  void DeleteFunction();
  FunctionStatus status_ = FunctionStatus::NotActivated;
  Events event_pool_;
  bool is_initialized_ = false;
  bool need_to_delete_ = false;
 private:
  FunctionType type_;
  int id_;
  static int id_setter_;
  // if parent exists, it ca not be deleted earlier, than child
  FunctionBasement* parent_ = nullptr;
};

// can be used as timer by plasing
// target_component_ = consts::fake_component
class PhysicalFunction : public FunctionBasement {
 public:
  PhysicalFunction(FunctionBasement* parent = nullptr)
    : FunctionBasement(FunctionType::Physical, parent),
      function_signature_("") {}
  PhysicalFunction(UnificatedArguments&& target_components,
                   FunctionBasement* parent = nullptr);
  virtual void FunctionCalled() override final;
  const std::string& target_component() const { return target_component_; }
  FunctionSignature& function_signature() { return function_signature_; }
  static PhysicalFunction* Timer(int life_tyme_ms,
                                FunctionBasement* parent = nullptr);
 private:
  std::string target_component_; // first arg
  int left_lifetime_; // second arg
  FunctionSignature function_signature_; // all that left
};

class FunctionFactory;

class StandartFunction : public FunctionBasement {
  friend class FunctionFactory;

 public:
  virtual bool check_state() override;
  StandartFunction() : FunctionBasement(FunctionType::Standart) {}
  StandartFunction(UnificatedArguments&& target_components,
                   FunctionBasement* parent = nullptr);
  virtual void FunctionCalled() override;
  virtual void ChildFunctionStatusUpdated(FunctionBasement *child_func) override;
  Functions& GetChildFunctions();
  template<class FuncType, class... Args>
  void AddBodyFunction(Args... args) {
    auto converted_args = ToFuncArgs(args...);
    body_funcs_.insert(new FuncType(converted_args, this));
  }
  template<class FuncType, class... Args>
  void AddTailFunction(Args... args) {
    auto converted_args = ToFuncArgs(std::forward(args...));
    body_funcs_.insert(new FuncType(converted_args, this));
  }
 protected:
  /// checks state onse when called
  StaticTrigger static_trigger_;
  Functions body_funcs_;
  // function finished when it's tail finished
  Functions tail_funcs_;
};


// sets some triggers
class TriggerFunction : public FunctionBasement {
 public:
  TriggerFunction() : FunctionBasement(FunctionType::Trigger) {}
};

#endif // BASEFUNCTION_H
