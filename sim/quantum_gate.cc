#include "quantum_gate.h"

namespace Qwality {

QuantumGate::QuantumGate(int n_qubits) : n(n_qubits), dim(1 << n), values(dim, dim) {
  //have to make identity matrix here
  //because matrix(r, c) generates 0-matrix
  for(int i = 0; i < dim; i++) {
    values(i, i) = 1;
  }
}

//TODO JB
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

//TODO JB
QuantumGate QuantumGate::tensor(const QuantumGate& other) const {}

//TODO JB
void QuantumGate::tensor_(const QuantumGate& other) {}

//TODO JB
QuantumGate QuantumGate::andThen(const QuantumGate& next) const {}

//TODO JB
void QuantumGate::andThen_(const QuantumGate& next) {}

//TODO JB
QuantumGate QuantumGate::conjugate() const {}

//TODO JB
void QuantumGate::conjugate_() {}

//TODO JB (make sure this is correct)
QuantumGate I() {
  matrix I{{1, 0}, {0, 1}};
  return I;
}

//TODO JB
QuantumGate H() {}

//TODO JB
QuantumGate X() {}

//TODO JB
QuantumGate Y() {}

//TODO JB
QuantumGate Z() {}

//TODO JB
QuantumGate R(int k) {}

//TODO JB
QuantumGate control(const QuantumGate& gate) {}

//TODO JB
QuantumGate swap(vector<int>& indices) {}

}