#include <iostream>
#include "core.h"
#include <ctime>

// TODO implement logger
// TOTO doxigen doc

int main() {
  auto& core = Core::getCore();
  core.TestSignal("test");
  core.TestSignal("anime destroys life");
  int dummy;
  std::cin >> dummy;
}
