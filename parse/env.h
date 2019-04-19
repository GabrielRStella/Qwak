#ifndef ENV_H
#define ENV_H

#include <string>
#include <iostream>

#include <unordered_map>
#include <stack>
#include <vector>

#include "../sim/quantum_state.h"
#include "../sim/quantum_gate.h"

namespace Qwak {

using Qwality::QuantumState;
using Qwality::QuantumGate;

using std::vector;

const int DATATYPE_NONE = 0;
const int DATATYPE_STATE = 1; //quantum state
const int DATATYPE_GATE = 2;
const int DATATYPE_INT = 3;
const int DATATYPE_EX = 4;
const int DATATYPE_CLASSICAL = 5; //classical state
const int DATATYPE_LIST = 10;

//represents an object in a quantum environment
//has type and data
//TODO: could have implementor subtypes for each programmable type
//so that the implementors could virtual-override all necessary interfaces
class Object {
private:
  int type;
  void* data;
public:
  Object(); //DATATYPE_NONE, nullptr
  Object(int type, void* data);

  operator bool() const; //checks if type != DATATYPE_NONE and data != nullptr

  int getType() const;
  void* getData() const;

  void setData(int type, void* data);
};

const Object OBJECT_NONE;

//encapsulates the quantum state and other variables within a function
class Environment {
private:
  QuantumState state;
  Object state_;

  //scoped list of variables (Objects)
  std::stack<std::unordered_map<std::string, Object>> variables;
  int scopeLevel;

public:
  Environment();

  QuantumState& getState();

  //create or modify variable
  Object& operator[](const std::string& variable);

  //when an Environment gets pushed to a new scope, it keeps the quantum state, but wipes all variables/references
  int getScopeLevel();
  int push(); //returns the scope level before pushing
  int pop(); //returns the scope level after popping, thus push() == pop() from same level

//operations
public:
  Object applyOperator(const std::string& op, Object left, Object right);
  Object applyGate(Object gate, Object state);
  Object applyGate(Object gate, Object state, const vector<int>& qubits);

  Object createObject(const QuantumState& state);
  Object createObject(const QuantumGate& gate);
  Object createObject(int value);
};

} //end namespace

std::ostream& operator<<(std::ostream& os, Qwak::Object o);

#endif //ENV_H