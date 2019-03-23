#ifndef QUANTUM_STATE_H
#define QUANTUM_STATE_H

#include <vector>

#include <ginac/ginac.h>

#include "quantum_gate.h"

namespace Qwality {

using std::vector;

using GiNaC::ex;
using GiNaC::matrix;

//TODO: define API for creating and manipulating quantum states

class QuantumState {
private:
  //TODO: data members

  int n;

  //column vector of quantum state
  matrix amplitudes;

public:
  //TODO:
  //accessors and modifiers

  QuantumState(int n_qubits);
  QuantumState(const QuantumState& cp);

  QuantumState& operator=(const QuantumState& cp);

  int getN() const;
  matrix& getAmplitudes();
  const matrix& getAmplitudes() const;
  double getProbability(int qubit) const;

  int sample() const; //randomly samples the entire state and produces a measurement
  QuantumState measure() const;
  void measure_();

  int sample(int qubit) const; //randomly samples a single qubit and produces a measurement
  //will collapse one qubit and return entire new state
  QuantumState measure(int qubit) const;
  void measure_(int qubit);

  //resets state to |0^n>
  void reset();

  bool isEntangled() const;

  //TODO: make sure non-const states can call the const function overload

  //tensor two states together, this tensor that
  QuantumState tensor(const QuantumState& other) const;
  void tensor_(const QuantumState& other);

  //decompose this vector as much as possible into un-tensored states
  void untensor(vector<QuantumState>& receiver) const;
  vector<QuantumState> untensor() const;

  /*
    TODO: apply the gate to a part of this state, and return the modified state

    The given vector of qubits determines which parts of this state (and in what order)
    to apply the gate.
  */
  QuantumState applyPartial(const QuantumGate& gate, const std::vector<std::size_t>& qubits) const;
  void applyPartial_(const QuantumGate& gate, const std::vector<std::size_t>& qubits);

  //simple matrix multiplication
  QuantumState applyFull(const QuantumGate& gate) const;
  void applyFull_(const QuantumGate& gate);

  //TODO: define arithmetic operators?
  //might require sub-state operator (like H*a[{2}]) where a[2] returns a sub-state containing info about what qubits to use
  //also automatic type conversions e.g. QuantumState -> int to automatically sample

};

}

#endif //QUANTUM_STATE_H