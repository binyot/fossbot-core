#ifndef PHYSICALCOMPONENTMANAGER_H
#define PHYSICALCOMPONENTMANAGER_H

#include <map>
#include <string>
#include <memory>
#include <thread>
#include <vector>

#include "base_component.h"
#include "consts.h"
#include "physical_component.h"
#include "base_function.h"

class PhysicalComponentsManager : public BasicEventsManager
{
 public:
  static PhysicalComponentsManager& getManager() { //singleton
    static PhysicalComponentsManager manager;
    return manager;
  }
  PhysicalComponentsManager(PhysicalComponentsManager const&) = delete;
  void operator=(PhysicalComponentsManager const&) = delete;
  static void test_event_loop(const unsigned int update_interval_ms);
  virtual void HandleFunction(std::unique_ptr<FunctionBasement> & function) override;
  virtual ~PhysicalComponentsManager();

private:
  PhysicalComponentsManager();
  static std::map<std::string,  PhysicalComponent*> component_map_;
};

#endif // PHYSICALCOMPONENTMANAGER_H
