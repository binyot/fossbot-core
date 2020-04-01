#ifndef FUNCTIONSMANAGER_H
#define FUNCTIONSMANAGER_H

#include <map>
#include <string>
#include <memory>
#include <thread>
#include <vector>
#include <iostream>

#include "base_component.h"
#include "base_function.h"
#include "parsers.h"
#include "physical_components_manager.h"
#include "function_factory.h"

class FunctionsManager : public BasicEventsManager
{
 public:
  using EventWatchers = std::vector<TriggerFunction>;

  static FunctionsManager& getManager() { //singleton
    static FunctionsManager manager;
    return manager;
  }
  FunctionsManager(FunctionsManager const&) = delete;
  void operator=(FunctionsManager const&) = delete;
  bool AddFunction(); //TODO parse and add new func type
  void SentEvents([[maybe_unused]] FunctionBasement::Events events) {} //TODO
  virtual bool CallFunction(FunctionSignature func_signature) override;
  virtual void HandleFunction(std::unique_ptr<FunctionBasement>& function) override;
  virtual ~FunctionsManager();

private:
  PhysicalComponentsManager& physical_manager_() {
    return PhysicalComponentsManager::getManager(); }
  FunctionsManager();
  FunctionFactory functions_;
  std::map<EventSignature::EventAndSender, EventWatchers> event_map_;
};

#endif // FUNCTIONSMANAGER_H
