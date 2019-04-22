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

  QuantumGate tensor(int n) const;
  void tensor_(int n);

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
  static QuantumGate kcontrol(const QuantumGate&, int);
  static QuantumGate swap(const vector<int>& indices);
  template<typename F>
  static QuantumGate makeUnitary(F func); //F: {0,1} -> {0,1}
  template<typename F>
  static QuantumGate makeUnitary(F func, int n, int m); //F: {0,1}^n -> {0,1}^m
  
  friend std::ostream& ::operator<<(std::ostream& o, const QuantumGate& qg);

  //TODO: k-controlled gates?

  //TODO: overload arithmetic operators?
};

//F: {0,1} -> {0,1}
template<typename F>
QuantumGate QuantumGate::makeUnitary(F func) {
  matrix U_f(4,4);
  for(int x = 0; x < 2; ++x) {
    for(int y = 0; y < 2; ++y) {
      // Generate matrix of the form |x>|y xor f(x)><x|<y|.
      U_f((x << 1) | (y != func(x)), (x << 1) | y) = 1;
    }
  }
  return QuantumGate(2,U_f);
}

//F: {0,1}^n -> {0,1}^m
template<typename F>
QuantumGate QuantumGate::makeUnitary(F func, int n, int m) {
  QWALITY_NOT_SUPPORTED
}

} //end namespace

#endif //QUANTUM_GATE_H
