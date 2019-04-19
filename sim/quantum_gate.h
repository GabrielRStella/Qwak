#ifndef QUANTUM_GATE_H
#define QUANTUM_GATE_H

#include <ginac/ginac.h>

#include "util.h"

namespace Qwality {
    class QuantumGate;
}

std::ostream& operator<<(std::ostream& o, const Qwality::QuantumGate& qg);

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

  //inverse of gate
  QuantumGate inverse() const;
  void inverse_();

  //complex conjugate of gate
  QuantumGate conjugate() const;
  void conjugate_();

  static const QuantumGate I;
  static const QuantumGate H;
  static const QuantumGate X;
  static const QuantumGate Y;
  static const QuantumGate Z;
  static QuantumGate R(int);
  static QuantumGate control(const QuantumGate&);
  static QuantumGate swap(const vector<int>& indices);
  
  friend std::ostream& ::operator<<(std::ostream& o, const QuantumGate& qg);

  //TODO: k-controlled gates?

  //TODO: overload arithmetic operators?
};

} //end namespace

#endif //QUANTUM_GATE_H
