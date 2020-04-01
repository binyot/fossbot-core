#ifndef CORE_H
#define CORE_H

#include <string>
#include <map>
#include <memory>

#include "consts.h"
#include "base_component.h"
#include "emergency_situation_manager.h"
#include "functions_manager.h"

class Core
{
 using ComponentMap = std::map<std::string, std::unique_ptr<BaseComponent>>;

 public:
  static Core& getCore() { //singleton
    static Core core;
    return core;
  }
  void TestSignal(std::string func_name);
  /// add foregin component such neural netrk or another module
  bool AddComponent(BaseComponent component, std::string component_name);
  Core(Core const&) = delete;
  void operator=(Core const&) = delete;
  void CallFunction(const std::string &call);
 private:
  Core();
  [[noreturn]] void HandleManagers();
  std::thread event_loop_;
  ComponentMap component_map_;
  EmergencySituationManager& emegrensy_manager() {
    assert(false && "not implemented");
    return *(new EmergencySituationManager()); }
  FunctionsManager& functions_manager() {
    return *static_cast<FunctionsManager*>(
          component_map_.at(consts::function_manager_name).get()); }
  PhysicalComponentsManager& physical_manager() {
    return PhysicalComponentsManager::getManager(); }
};


#endif // CORE_H
