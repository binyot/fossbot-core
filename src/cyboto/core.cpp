#include "core.h"
#include <iostream>

Core::Core() {
  BaseComponent::SetCore(this);
  component_map_.emplace(consts::function_manager_name, &FunctionsManager::getManager());
  //event_loop_ = std::thread(&Core::HandleManagers, this);
  //debug
  //HandleManagers();
}

void Core::TestSignal(std::string func_name) {
  if (func_name == "test") {
    std::cout << "Test func called, smth done... (not implemented)" << std::endl;
  } else {
    std::cout << "Test func called, to call some logic"
                 " send 'test' as string argument. Current agrument is: '"
              << func_name << "'." << std::endl;
  }
}

void Core::HandleManagers() {
  while(true) {
    const auto start = std::chrono::steady_clock::now();
    functions_manager().ProcessActiveFunctions();
    physical_manager().ProcessActiveFunctions();
    // TODO handle events there

    auto func_exec_duration = std::chrono::milliseconds(consts::atomic_time_value)
                              - (std::chrono::steady_clock::now() - start);
    if (std::chrono::milliseconds(consts::atomic_time_value)
           < std::chrono::steady_clock::now() - start) {
      std::cout << "Elapsed time bigger than atomic_time_value!" << std::endl;
    }
    std::this_thread::sleep_for(func_exec_duration);
  }
}

void Core::CallFunction(const std::string &call)
{
  functions_manager().CallFunction(call);
}
