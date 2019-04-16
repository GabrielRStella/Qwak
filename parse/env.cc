#include "env.h"

namespace Qwak {

Environment::Environment() : state(0), variables(), scopeLevel(0) {}

Object& Environment::operator[](const std::string& variable) {
  return variables.top()[variable];
}

int Environment::getScopeLevel() {
  return scopeLevel;
}

int Environment::push() {
  variables.emplace(); //add default-constructed (empty) map
  return scopeLevel++;
}

int Environment::pop() {
  variables.pop();
  return --scopeLevel;
}

} //end namespace