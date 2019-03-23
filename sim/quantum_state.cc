#include "quantum_state.h"

namespace Qwality {

//TODO
QuantumState::QuantumState(int n_qubits) {}

//TODO
QuantumState::QuantumState(const QuantumState& cp) {}

//TODO
QuantumState::QuantumState& operator=(const QuantumState& cp) {}

int QuantumState::getN() const {
  return n;
}

matrix& QuantumState::getAmplitudes() {
  return amplitudes;
}

const matrix& QuantumState::getAmplitudes() const {
  return amplitudes;
}

//TODO
double QuantumState::getProbability(int qubit) const {}

//TODO
int QuantumState::sample() const {}

//TODO
QuantumState QuantumState::measure() const {}

//TODO
void QuantumState::measure_() {}

//TODO
int QuantumState::sample(int qubit) const {}

//TODO
QuantumState QuantumState::measure(int qubit) const {}

//TODO
void QuantumState::measure_(int qubit) {}

//TODO
void QuantumState::reset() {}

//TODO
bool QuantumState::isEntangled() const {}

//TODO
QuantumState QuantumState::tensor(const QuantumState& other) const {}

//TODO
void QuantumState::tensor_(const QuantumState& other) {}

//TODO
void QuantumState::untensor(vector<QuantumState>& receiver) const {}

//TODO
vector<QuantumState> QuantumState::untensor() const {}

//TODO
QuantumState QuantumState::applyPartial(const QuantumGate& gate, const std::vector<std::size_t>& qubits) const {}

//TODO
void QuantumState::applyPartial_(const QuantumGate& gate, const std::vector<std::size_t>& qubits) {}

//TODO
QuantumState QuantumState::applyFull(const QuantumGate& gate) const {}

//TODO
void QuantumState::applyFull_(const QuantumGate& gate) {}

}