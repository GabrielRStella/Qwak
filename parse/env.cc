#include "env.h"

namespace Qwak {

//OPERATORS

//Object operator_(Environment& e, Object left, Object right) {}

Object operator_plus(Environment& e, Object left, Object right) {
  return Object(DATATYPE_INT, new int(left.castData<int>() + right.castData<int>()));
}

Object operator_minus(Environment& e, Object left, Object right) {
  return Object(DATATYPE_INT, new int(left.castData<int>() - right.castData<int>()));
}

Object operator_tensor_exponent_gate(Environment& e, Object left, Object right) {
  QuantumGate& g1 = left.castData<QuantumGate>();
  int j = right.castData<int>();
  QuantumGate* g2 = new QuantumGate(0);

  for(int i = 0; i < j; i++) g2->tensor_(g1);

  return Object(DATATYPE_GATE, g2);
}

Object operator_tensor_exponent_state(Environment& e, Object left, Object right) {
  //TODO
  return OBJECT_NONE;
}

Object operator_exponent_gate(Environment& e, Object left, Object right) {
  QuantumGate& g1 = left.castData<QuantumGate>();
  int j = right.castData<int>();
  QuantumGate* g2 = new QuantumGate(g1.getN());

  for(int i = 0; i < j; i++) g2->andThen_(g1);

  return Object(DATATYPE_GATE, g2);
}

Object operator_exponent_int(Environment& e, Object left, Object right) {
  int i = left.castData<int>();
  int j = right.castData<int>();
  int result = 1;
  for(int n = 0; n < j; n++) result *= i;
  return e.createObject(result);
}

Object operator_tensor_gate(Environment& e, Object left, Object right) {
  QuantumGate& g1 = left.castData<QuantumGate>();
  QuantumGate& g2 = right.castData<QuantumGate>();
  return e.createObject(g1.tensor(g2));
}

Object operator_tensor_state(Environment& e, Object left, Object right) {
  vector<int>& s1 = left.castData<vector<int>>();
  vector<int>& s2 = right.castData<vector<int>>();
  vector<int>* s = new vector<int>();
  for(int i : s1) s->push_back(i);
  for(int i : s2) s->push_back(i);
  return Object(DATATYPE_STATE, s);
}

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
  state_ = Object(DATATYPE_STATE, new vector<int>());
  variables.emplace(); //add first level of scope
  //auto-variables
  constants["I"] = createObject(QuantumGate::I);
  constants["H"] = createObject(QuantumGate::H);
  constants["X"] = createObject(QuantumGate::X);
  constants["Y"] = createObject(QuantumGate::Y);
  constants["Z"] = createObject(QuantumGate::Z);
  //operators: + - ^** ** ^*
  operators[string("+")][DATATYPE_INT][DATATYPE_INT] = &operator_plus;
  operators[string("-")][DATATYPE_INT][DATATYPE_INT] = &operator_minus;
  operators[string("^**")][DATATYPE_GATE][DATATYPE_INT] = &operator_tensor_exponent_gate;
  operators[string("^**")][DATATYPE_STATE][DATATYPE_INT] = &operator_tensor_exponent_state;
  operators[string("**")][DATATYPE_GATE][DATATYPE_INT] = &operator_exponent_gate;
  operators[string("**")][DATATYPE_INT][DATATYPE_INT] = &operator_exponent_int;
  operators[string("^*")][DATATYPE_GATE][DATATYPE_GATE] = &operator_tensor_gate;
  operators[string("^*")][DATATYPE_STATE][DATATYPE_STATE] = &operator_tensor_state;
  //TODO: equality
  //operators[string("==")][DATATYPE_STATE][DATATYPE_STATE] = &operator_equals_state;
}

QuantumState& Environment::getState() {
  return state;
}

Object& Environment::operator[](const string& variable) {
  if(variable == "STATE") return state_;
  if(constants.find(variable) != constants.end()) return constants[variable];
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

Object Environment::applyOperator(const string& op, Object left, Object right) {
  auto a1 = operators[op];
  auto a2 = a1[left.getType()];
  auto a3 = a2[right.getType()];

  Operator o = operators[op][left.getType()][right.getType()];
  if(o) {
    return o(*this, left, right);
  } else {
    return OBJECT_NONE;
  }
}

Object Environment::applyGate(Object gate, Object state) {
printf("la\n");
  QuantumGate& gate2 = gate.castData<QuantumGate>();
  vector<int>& val = state.castData<vector<int>>();
  this->state.applyPartial_(gate2, val);
  return state;
}

Object Environment::applyGate(Object gate, Object state, const vector<int>& qubits) {
  QuantumGate& gate2 = gate.castData<QuantumGate>();
  vector<int>& val = state.castData<vector<int>>();
  vector<int> qubits3;
  //qubits have to be inserted in reverse order for some reason...
printf("lee: %d\n", int(qubits.size()));
  for(int i = qubits.size() - 1; i >= 0; i--) qubits3.push_back(val[qubits[i]]);

printf("le\n");
  this->state.applyPartial_(gate2, qubits3);
  return state;
}

Object Environment::createObject(const QuantumState& state) {
  //will actually be a sub-state of the entire state
  int n = this->state.getN();
  int m = n + state.getN();
  this->state.tensor_(state);
  vector<int>* val = new vector<int>();
  vector<int>& val2 = state_.castData<vector<int>>();
  for(int i = n; i < m; i++) {
    val->push_back(i);
    val2.push_back(i);
  }
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
      {
        std::vector<int>& val = o.castData<std::vector<int>>();
        os << "Substate[";
        for(int i = 0; i < val.size(); i++) {
          if(i) os << ", ";
          os << val[i];
        }
        return os << "]";
      }
    case Qwak::DATATYPE_GATE:
      return os << o.castData<Qwality::QuantumGate>();
    case Qwak::DATATYPE_INT:
      return os << o.castData<int>();
    default:
      return os << "?";
  }
}