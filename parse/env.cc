#include "env.h"

namespace Qwak {

//object

Object::Object() : type(DATATYPE_NONE), data(nullptr) {}

Object::Object(int type_, void* data_) : type(type_), data(data_) {}

Object::operator bool() const {
  return type != DATATYPE_NONE && data;
}

int Object::getType() const {
  return type;
}

void* Object::getData() const {
  return data;
}

void Object::setData(int type, void* data) {
  this->type = type;
  this->data = data;
}

//environment

Environment::Environment() : state(0), variables(), scopeLevel(0) {
  //"STATE" pseudo-variable
  int* val = new int[2];
  val[0] = 0;
  val[1] = 0;
  state_ = Object(DATATYPE_STATE, val);
  //auto-variables
  (*this)["H"] = createObject(QuantumGate::H);
}

Object& Environment::operator[](const std::string& variable) {
  if(variable == "STATE") return state_;
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

//modifiers

Object Environment::applyOperator(const std::string& op, Object left, Object right) {
  
}

Object Environment::applyGate(Object gate, Object state) {
  QuantumGate& gate2 = *(QuantumGate*)gate.getData();
  int* val = (int*)state.getData();
  vector<int> qubits;
  for(int i = val[0]; i < val[1]; i++) qubits.push_back(i);
  this->state.applyPartial_(gate2, qubits);
  return state;
}

Object Environment::applyGate(Object gate, Object state, const vector<int>& qubits) {
  QuantumGate& gate2 = *(QuantumGate*)gate.getData();
  int* val = (int*)state.getData();
  vector<int> qubits2;
  for(int i = val[0]; i < val[1]; i++) qubits2.push_back(i);
  vector<int> qubits3;
  for(int i = 0; i < qubits.size(); i++) qubits3.push_back(qubits2[qubits[i]]);

  this->state.applyPartial_(gate2, qubits3);
  return state;
}

Object Environment::createObject(const QuantumState& state) {
  //will actually be a sub-state of the entire state
  int n = this->state.getN();
  int m = n + state.getN();
  this->state.tensor_(state);
  ((int*)(state_.getData()))[1] = m;
  int* val = new int[2];
  val[0] = n;
  val[1] = m;
  return Object(DATATYPE_STATE, val);
}

Object Environment::createObject(const QuantumGate& gate) {
  return Object(DATATYPE_GATE, new QuantumGate(gate));
}

Object Environment::createObject(int value) {
  return Object(DATATYPE_INT, new int(value));
}

} //end namespace

std::ostream& operator<<(std::ostream& os, Qwak::Object o) {
  switch(o.getType()) {
    case Qwak::DATATYPE_NONE:
      return os << "NONE";
    case Qwak::DATATYPE_STATE:
      return os << "Substate[" << (((int*)o.getData())[0]) << ", " << (((int*)o.getData())[1]) << "]";
    case Qwak::DATATYPE_GATE:
      return os << (*(Qwality::QuantumGate*)o.getData());
    case Qwak::DATATYPE_INT:
      return os << (*(int*)o.getData());
    default:
      return os << "?";
  }
}