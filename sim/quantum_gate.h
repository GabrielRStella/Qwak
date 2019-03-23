#ifndef QUANTUM_GATE_H
#define QUANTUM_GATE_H

#include <ginac/ginac.h>

#include "util.h"

namespace Qwality {

using std::vector;

using GiNaC::ex;
using GiNaC::matrix;

//TODO: define API for creating and manipulating quantum logic gates

class QuantumGate {
private:
  int n;
  matrix values;

public:
  QuantumGate(int n_qubits); //generate n-qubit identity
  QuantumGate(matrix& values);

  int getN() const;

  const matrix& getValues() const;
  matrix& getValues();

  //tensor two gates together, this tensor that
  QuantumGate tensor(const QuantumGate& other) const;
  void tensor_(const QuantumGate& other);

  //concatenates two operations
  QuantumGate andThen(const QuantumGate& next) const;
  void andThen_(const QuantumGate& next);

  QuantumGate conjugate() const;
  void conjugate_();

  //TODO: overload arithmetic operators?
};

//basic operators

//identity
const QuantumGate& I();
//hadamard
const QuantumGate& H();
//pauli operators
const QuantumGate& X();
const QuantumGate& Y();
const QuantumGate& Z();

//phase rotation gate
QuantumGate R(int k);
//inserts a control qubit at position 0 (i.e. control <tensor> gate)
QuantumGate control(const QuantumGate& gate);
//constructs an n-qubit swap gate, swapping {1, 2, ..., n} to the positions in the given vector
QuantumGate swap(vector<int>& indices);

//TODO: k-controlled gates?
}

#endif //QUANTUM_GATE_H