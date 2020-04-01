#ifndef BASECOMPONENT_H
#define BASECOMPONENT_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <thread>
#include <map>

#include "consts.h"
#include "parsers.h"
#include "base_function.h"

class Core;

class BaseComponent
{
 public:
  using FunctionCall = std::pair<std::string, FunctionSignature>; //
  using FunctionCalls = std::vector<FunctionCall>;
  using FunctionsForUser
        = std::map<std::string,
                 std::function<FunctionCalls(UnificatedArguments)>>;

  /// parse signature and call needed function
  virtual bool CallFunction(FunctionSignature func_signature);
  std::vector<std::string> AvailableFunctions();
  static void SetCore(Core* core) { core_ = core; }
  virtual ~BaseComponent();

protected:
  FunctionsForUser functions_for_user_;
  static Core* core_;
};

class BasicEventsManager : public BaseComponent {
 public:
  void AddActiveFunction(std::unique_ptr<FunctionBasement>& function);
  void AddActiveFunction(FunctionBasement* function);
  void ProcessActiveFunctions();
  virtual ~BasicEventsManager();

protected:
  virtual void HandleFunction(std::unique_ptr<FunctionBasement>& function);
 private:
  void MoveFunctionsPoolToMainArray();
  std::vector<std::unique_ptr<FunctionBasement>> active_functions_pool_;
  std::vector<std::unique_ptr<FunctionBasement>> active_functions_;
};

#endif // BASECOMPONENT_H
