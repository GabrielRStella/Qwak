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

QuantumGate QuantumGate::andThen(const QuantumGate& next) const {
	return QuantumGate(n, next.values.mul(values));
}

void QuantumGate::andThen_(const QuantumGate& next) {
	values = next.values.mul(values);
}

QuantumGate QuantumGate::conjugate() const {
	return QuantumGate(n, conjugate_transpose(values));
}

void QuantumGate::conjugate_() {
	values = conjugate_transpose(values);
}

QuantumGate I() {
  // matrix I{{1, 0}, {0, 1}};
  return QuantumGate(1);
}

QuantumGate H() {
  matrix H{{1, 1}, {1, -1}};
  return QuantumGate(1, H.mul_scalar(ex(1)/sqrt(ex(2))));
}

QuantumGate X() {
	matrix X{{0, 1}, {1, 0}};
	return QuantumGate(1, X);
}

QuantumGate Y() {
	matrix Y{{0, -I}, {I, 0}};
	return QuantumGate(1, Y);
}


QuantumGate Z() { 
	matrix Z{{1, 0}, {0, -I}};
	return QuantumGate(1, Z);
}

QuantumGate R(int k) { 
	matrix R_k{{1, 0}, {0, pow(Euler,(2*Pi*I/(pow(2,k))))}};
	return QuantumGate(1, R_k);
}

QuantumGate control(const QuantumGate& gate) {
	QuantumGate result(gate.n + 1);
	for(int i = 0; i < gate.n; ++i) {
		for(int j = 0; j < gate.n; ++j) {
			result.values(j + gate.n, i + gate.n) = gate.values(j,i);
		}
	}
	return result;
}

QuantumGate swap(vector<int>& indices) {}

}
