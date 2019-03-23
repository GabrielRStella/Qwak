#include "quantum_gate.h"

namespace Qwality {

QuantumGate::QuantumGate(int n_qubits) : n(n_qubits), dim(1 << n), matrix(dim, dim) {
  //have to make identity matrix here
  //because matrix(r, c) generates 0-matrix
  for(int i = 0; i < dim; i++) {
    matrix(i, i) = 1;
  }
}

//TODO
QuantumGate::QuantumGate(matrix& values) {}

int QuantumGate::getN() const {
  return n;
}

const matrix& QuantumGate::getValues() const {
  return values;
}

matrix& QuantumGate::getValues() {
  return values;
}

//TODO
QuantumGate QuantumGate::tensor(const QuantumGate& other) const {}

//TODO
void QuantumGate::tensor_(const QuantumGate& other) {}

//TODO
QuantumGate QuantumGate::andThen(const QuantumGate& next) const {}

//TODO
void QuantumGate::andThen_(const QuantumGate& next) {}

//TODO
QuantumGate QuantumGate::conjugate() const {}

//TODO
void QuantumGate::conjugate_() {}

//TODO
const QuantumGate& I() {}

//TODO
const QuantumGate& H() {}

//TODO
const QuantumGate& X() {}

//TODO
const QuantumGate& Y() {}

//TODO
const QuantumGate& Z() {}

//TODO
QuantumGate R(int k) {}

//TODO
QuantumGate control(const QuantumGate& gate) {}

//TODO
QuantumGate swap(vector<int>& indices) {}

}