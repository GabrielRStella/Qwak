#include "quantum_state.h"

namespace Qwality {

QuantumState::QuantumState(int n_qubits, int state) : n(n_qubits), dim(1 << n), amplitudes(dim, 1) {
  //set proper element of amplitudes to 1 (based on "state" parameter)
  amplitudes(state, 0) = 1;
}

//TODO GS
QuantumState::QuantumState(const QuantumState& cp) {}

//TODO GS
QuantumState& QuantumState::operator=(const QuantumState& cp) {}

int QuantumState::getN() const {
  return n;
}

matrix& QuantumState::getAmplitudes() {
  return amplitudes;
}

const matrix& QuantumState::getAmplitudes() const {
  return amplitudes;
}

//TODO GS
double QuantumState::getProbability(int qubit) const {}

//TODO GS
int QuantumState::sample() const {}

//TODO GS
QuantumState QuantumState::measure() const {}

//TODO GS
void QuantumState::measure_() {}

//TODO GS
int QuantumState::sample(int qubit) const {}

//TODO GS
QuantumState QuantumState::measure(int qubit) const {}

//TODO GS
void QuantumState::measure_(int qubit) {}

//TODO GS
void QuantumState::reset() {}

//TODO GS
bool QuantumState::isEntangled() const {}

//TODO GS
QuantumState QuantumState::tensor(const QuantumState& other) const {}

//TODO GS
void QuantumState::tensor_(const QuantumState& other) {}

//TODO GS
void QuantumState::untensor(vector<QuantumState>& receiver) const {}

//TODO GS
vector<QuantumState> QuantumState::untensor() const {}

//TODO GS
QuantumState QuantumState::applyPartial(const QuantumGate& gate, const std::vector<std::size_t>& qubits) const {}

//TODO GS
void QuantumState::applyPartial_(const QuantumGate& gate, const std::vector<std::size_t>& qubits) {}

//TODO GS
QuantumState QuantumState::applyFull(const QuantumGate& gate) const {}

//TODO GS
void QuantumState::applyFull_(const QuantumGate& gate) {}

}