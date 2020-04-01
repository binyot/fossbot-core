#include "functions_manager.h"
#include "consts.h"
#include "common.h"

FunctionsManager::FunctionsManager() {
  PhysicalComponentsManager::getManager(); //init
  //test
  //CallFunction({ToFuncArgs("ExampleRotate3Motors", 10, -0.03, 30, 0.03, 50, 0.02)});
  //CallFunction({ToFuncArgs("ExampleWithTailAndDelay", 10000 /*10sec*/)});
  std::cout << "FunctionsManager created" << std::endl;
}

bool FunctionsManager::CallFunction(FunctionSignature func_signature) {
  // TODO check if function exists
  AddActiveFunction(static_cast<FunctionBasement*>(
                        functions_.GetFunction(func_signature)));
  return true;
}

void FunctionsManager::HandleFunction(std::unique_ptr<
                                          FunctionBasement>& function) {
  if (function->type() == FunctionType::Physical) {
    assert(false && "physical function not in physical manager");
  } else if (function->type() == FunctionType::Standart) {
    auto std_func = dynamic_cast<StandartFunction*>(function.get());
    // function initialisation
    if (std_func->status() == FunctionStatus::NotActivated
        || std_func->status() == FunctionStatus::BodyCompleted) {
      for (auto child_func : std_func->GetChildFunctions()) {
        if (child_func->type() == FunctionType::Physical)
          physical_manager_().AddActiveFunction(child_func);
        else
          AddActiveFunction(child_func);
      }
    }
  }
}
FunctionsManager::~FunctionsManager()
{
}
