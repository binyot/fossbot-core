#include "base_component.h"

#include <chrono>
#include <mutex>
#include <iterator>
#include <iostream>
#include <algorithm>

Core* BaseComponent::core_ = nullptr;

bool BaseComponent::CallFunction(FunctionSignature func_signature) {
  assert(false && "not implemented"); return false;
}

BaseComponent::~BaseComponent()
{
}

void BasicEventsManager::MoveFunctionsPoolToMainArray() {
  if(!active_functions_pool_.empty()) {
    active_functions_.insert(active_functions_.end(),
                             std::make_move_iterator(active_functions_pool_.begin()),
                             std::make_move_iterator(active_functions_pool_.end()));
    active_functions_pool_.clear();
  }
}

void BasicEventsManager::ProcessActiveFunctions() {
  MoveFunctionsPoolToMainArray();
  auto function_handler = [this](auto & function) {
      function->FunctionCalled();
      HandleFunction(function);
      return function->need_to_delete();
  };
  active_functions_.erase(std::remove_if(active_functions_.begin(),
                                         active_functions_.end(),
                                         function_handler),
                          active_functions_.end());
}

void BasicEventsManager::AddActiveFunction(std::unique_ptr<FunctionBasement> & function) {
  active_functions_pool_.emplace_back(std::move(function));
}

void BasicEventsManager::AddActiveFunction(FunctionBasement* function) {
  active_functions_pool_.emplace_back(std::unique_ptr<FunctionBasement>(function));
}

void BasicEventsManager::HandleFunction(std::unique_ptr<FunctionBasement> & function) {
  assert(false && "This function must be implemented in child class");
}
BasicEventsManager::~BasicEventsManager()
{
}
