#ifndef QUANTUM_STATE_H
#define QUANTUM_STATE_H

#include <vector>
#include <iostream>

#include <ginac/ginac.h>

#include "quantum_gate.h"

//annoying forward declaration crap to resolve namespace issues...
namespace Qwality {
class QuantumState;
}
std::ostream& operator<<(std::ostream& o, const Qwality::QuantumState& qs);

namespace Qwality {

using std::vector;

using GiNaC::ex;
using GiNaC::matrix;

//TODO: define API for creating and manipulating quantum states

class QuantumState {
private:

  static int maskQubitOn(int qubit);
  static int maskQubitOff(int qubit);

  //number of qubits
  int n;
  int dim;

  //column vector of quantum state
  matrix amplitudes;

public:
  //TODO:
  //accessors and modifiers

  QuantumState(int n_qubits = 1, int state = 0); //initializes to |state>
  QuantumState(const QuantumState& cp);
  QuantumState(int n_qubits, matrix m);

  QuantumState& operator=(const QuantumState& cp);

  int getN() const;
  matrix& getAmplitudes();
  const matrix& getAmplitudes() const;
  ex getProbabilityOfState(int state) const;
  double getProbabilityDoubleOfState(int state) const;
  ex getProbabilityOfQubit(int qubit) const;
  double getProbabilityDoubleOfQubit(int qubit) const;

  void getProbabilities(vector<ex>& receiver) const;
  vector<ex> getProbabilities() const;

  void getProbabilityDoubles(vector<double>& receiver) const;
  vector<double> getProbabilityDoubles() const;

  int sample() const; //randomly samples the entire state and produces a measurement
  QuantumState measure() const;
  void measure_();

  bool sample(int qubit) const; //randomly samples a single qubit and produces a measurement
  //will collapse one qubit and return entire new state
  QuantumState measure(int qubit, bool* choice = nullptr) const;
  void measure_(int qubit, bool* choice = nullptr);

  //will collapse several qubit and return entire new state
  //the "choice" is returned in terms of the order given in the qubits vector
  QuantumState measure(const vector<int>& qubits, int* choice = nullptr) const;
  void measure_(const vector<int>& qubits, int* choice = nullptr);

  QuantumState zero(int qubit) const;
  void zero_(int qubit);
  QuantumState zero(const vector<int>& qubits) const;
  void zero_(const vector<int>& qubits);

  //resets state to |0^n>
  void reset();

  bool isEntangled() const;

  //tensor two states together, this tensor that
  QuantumState tensor(const QuantumState& other) const;
  void tensor_(const QuantumState& other);

  //decompose this vector as much as possible into un-tensored states
  void untensor(vector<QuantumState>& receiver) const;
  vector<QuantumState> untensor() const;

  /*
    apply the gate to a part of this state, and return the modified state

    The given vector of qubits determines which parts of this state (and in what order)
    to apply the gate.
  */
  QuantumState applyPartial(const QuantumGate& gate, const vector<int>& qubits) const;
  void applyPartial_(const QuantumGate& gate, const vector<int>& qubits);

  //simple matrix multiplication
  QuantumState applyFull(const QuantumGate& gate) const;
  void applyFull_(const QuantumGate& gate);

  //TODO: define arithmetic operators?
  //might require sub-state operator (like H*a[{2}]) where a[2] returns a sub-state containing info about what qubits to use
  //also automatic type conversions e.g. QuantumState -> int to automatically sample

  friend std::ostream& ::operator<<(std::ostream& o, const QuantumState& qs);

};

} //end namespace

#endif //QUANTUM_STATE_H
