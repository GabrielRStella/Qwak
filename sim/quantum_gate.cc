#include "quantum_gate.h"

namespace Qwality {

QuantumGate::QuantumGate(int n_qubits) : n(n_qubits), dim(1 << n), values(dim, dim) {
  //have to make identity matrix here
  //because matrix(r, c) generates 0-matrix
  for(int i = 0; i < dim; i++) {
    values(i, i) = 1;
  }
}

QuantumGate::QuantumGate(int n_qubits, const matrix& values_) : n(n_qubits), dim(1 << n), values(values_) {}

int QuantumGate::getN() const {
  return n;
}

const matrix& QuantumGate::getValues() const {
  return values;
}

matrix& QuantumGate::getValues() {
  return values;
}

QuantumGate QuantumGate::tensor(const QuantumGate& other) const {
	return QuantumGate(n + other.n, tensor_product(values, other.values));
}

void QuantumGate::tensor_(const QuantumGate& other) {
	values = tensor_product(values, other.values);
	n += other.n;
	dim <<= other.n;
}

//TODO JB
QuantumGate QuantumGate::andThen(const QuantumGate& next) const {}

//TODO JB
void QuantumGate::andThen_(const QuantumGate& next) {}

//TODO JB
QuantumGate QuantumGate::conjugate() const {}

//TODO JB
void QuantumGate::conjugate_() {}

QuantumGate I() {
  matrix I{{1, 0}, {0, 1}};
  return QuantumGate(1, I);
}

QuantumGate H() {
  matrix H{{1, 1}, {1, -1}};
  return QuantumGate(1, H.mul_scalar(ex(1)/sqrt(ex(2))));
}

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
