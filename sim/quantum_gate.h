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
  int dim;

  matrix values;

public:
  QuantumGate(int n_qubits); //generate n-qubit identity
  QuantumGate(int n_qubits, const matrix& values);

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

  static const QuantumGate I;
  static const QuantumGate H;
  static const QuantumGate X;
  static const QuantumGate Y;
  static const QuantumGate Z;
  static QuantumGate R(int);
  static QuantumGate control(const QuantumGate&);
  static QuantumGate swap(vector<int>& indices);

  //TODO: overload arithmetic operators?
};

//TODO: k-controlled gates?
}

#endif //QUANTUM_GATE_H
