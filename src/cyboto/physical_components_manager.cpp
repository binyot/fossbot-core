#include "physical_components_manager.h"

#include <ctime>
#include <iostream>
#include <chrono>
#include <mutex>

std::map<std::string,  PhysicalComponent*> PhysicalComponentsManager::component_map_;

/// inialization of all basic (physical) components
PhysicalComponentsManager::PhysicalComponentsManager() {
  component_map_.emplace(phys_comp::servo_m1, new ServoMotor(phys_comp::servo_m1, {-90, 90}));
  component_map_.emplace(phys_comp::servo_m2, new ServoMotor(phys_comp::servo_m2, {-90, 180}));
  component_map_.emplace(phys_comp::servo_m3, new ServoMotor(phys_comp::servo_m3, {0, 270}));
  std::cout << "PhysicalComponentsManager created" << std::endl;
}

void PhysicalComponentsManager::HandleFunction(std::unique_ptr<FunctionBasement>& function) {
  const auto physical_function = static_cast<PhysicalFunction*>(function.get());
  assert(physical_function && "wrong function type");
  // use fake_component as target components for timers
  if (physical_function->target_component() != consts::fake_component) {
    const auto component = component_map_.find(physical_function->target_component());
    assert(component != component_map_.end() && "component does not exist");
    component->second->CallFunctiuon(physical_function->function_signature());
    // debug
    //component->second->PrintAllInfo();
  }
  auto events = physical_function->ReceiveEvents();
// miliseconds
//    std::cout << "events receved at "
//              << std::chrono::duration_cast<std::chrono::seconds>
//                     (std::chrono::system_clock::now().time_since_epoch()).count()
//              << " event type:"
//              << events.front().GetEventAndSender().first << std::endl;
  if (!events.empty()) {
    auto curr_time = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(curr_time);
    std::cout << "event '" << events.front().GetEventAndSender().first
              <<"' receved at " << std::ctime(&end_time) << std::endl;
  }
}
PhysicalComponentsManager::~PhysicalComponentsManager()
{
}
