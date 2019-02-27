#ifndef QUANTUM_STATE_H
#define QUANTUM_STATE_H

//TODO: define API for creating and manipulating quantum states

class QuantumState {
private:
  //TODO: data members

public:
  //TODO:
  //constructor with n qubits
  //accessors and modifiers

  /*
    TODO: apply the gate to a part of this state, and return the modified state

    The given vector of qubits determines which parts of this state (and in what order)
    to apply the gate.
  */
  QuantumState apply(QuantumGate& gate, std::vector<std::size_t> qubits);
};

#endif //QUANTUM_STATE_H