#include "base_function.h"

#include <iostream>

int FunctionBasement::id_setter_ = 0;

FunctionBasement::FunctionBasement(FunctionType type,
                                   FunctionBasement* parent)
  : type_(type), parent_(parent) {
  id_ = id_setter_++;
}

FunctionBasement::Events FunctionBasement::ReceiveEvents() {
  auto buffer = event_pool_;
  event_pool_.clear();
  return buffer;
}

void FunctionBasement::DeleteFunction() {
  status_ = FunctionStatus::Finished;
  need_to_delete_ = true;
  if (parent_)
    parent_->ChildFunctionStatusUpdated(this);
}

void FunctionBasement::FunctionCalled() {
  assert(false && "must be called child class function");
}

void FunctionBasement::ChildFunctionStatusUpdated(FunctionBasement * /*child_func*/) {
    assert(false && "must be called child class function");
}

FunctionBasement::~FunctionBasement()
{
}

PhysicalFunction::PhysicalFunction(UnificatedArguments&& target_components,
                                   FunctionBasement* parent)
  : FunctionBasement(FunctionType::Physical, parent)
  , target_component_(target_components.NextArgument<std::string>())
  , left_lifetime_(target_components.NextArgument<int>())
  , function_signature_(target_components.raw_arguments()) {
  is_initialized_ = true;
}

void PhysicalFunction::FunctionCalled() {
  left_lifetime_ -= consts::atomic_time_value;
  if (left_lifetime_ <= 0) {
      DeleteFunction();
      event_pool_.push_back(EventSignature(
                              std::string(consts::function_finished) + "@1"));
  }
}

PhysicalFunction* PhysicalFunction::Timer(int life_tyme_ms,
                                         FunctionBasement* parent) {
  return new PhysicalFunction(ToFuncArgs(consts::fake_component, life_tyme_ms,
                                         consts::fake_function), parent);
}

StandartFunction::StandartFunction(UnificatedArguments&& target_components,
                 FunctionBasement* parent)
  : FunctionBasement(FunctionType::Standart, parent) {

}

bool StandartFunction::check_state() {
  return static_trigger_.Triggered();
}

void StandartFunction::FunctionCalled() {
  if (status() == FunctionStatus::TailCompleted)
    DeleteFunction();
}

void StandartFunction::ChildFunctionStatusUpdated(FunctionBasement* child_func) {
  if (child_func->need_to_delete()) {
    bool active_tail = static_cast<std::underlying_type_t<FunctionStatus>>(status())
                       > static_cast<std::underlying_type_t<
                             FunctionStatus>>(FunctionStatus::BodyCompleted);
    auto& set_to_search = active_tail ? tail_funcs_ : body_funcs_;
    auto body_iter = set_to_search.find(child_func);
    assert(body_iter != set_to_search.end() && "function not found");
    set_to_search.erase(body_iter);
    if (set_to_search.empty())
      status_ = active_tail ? FunctionStatus::TailCompleted
                            : FunctionStatus::BodyCompleted;
    if (status() == FunctionStatus::TailCompleted)
      DeleteFunction();
  }
}

FunctionBasement::Functions& StandartFunction::GetChildFunctions() {
  if (status() == FunctionStatus::NotActivated) {
    // break here
    assert(!body_funcs_.empty() && "must have at least 1 function");
    status_ = FunctionStatus::BodyRunning;
    return body_funcs_;
  }
  if (status() == FunctionStatus::BodyCompleted) {
    status_ = tail_funcs_.empty() ? FunctionStatus::TailCompleted
                                  : FunctionStatus::TailRunning;
    if (status() == FunctionStatus::TailCompleted)
      DeleteFunction();
    return tail_funcs_;
  }
  assert(false && "GetChildFunctions should not be called with such status");
  return body_funcs_;
}
