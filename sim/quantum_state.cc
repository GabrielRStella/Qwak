#include "quantum_state.h"

#include "util.h"

namespace Qwality {

using namespace GiNaC;

int QuantumState::maskQubitOn(int qubit) {
  return 1 << qubit;
}

int QuantumState::maskQubitOff(int qubit) {
  return ~QuantumState::maskQubitOn(qubit);
}

QuantumState::QuantumState(int n_qubits, matrix m) : n(n_qubits), dim(1 << n), amplitudes(m) {}

QuantumState::QuantumState(int n_qubits, int state) : n(n_qubits), dim(1 << n), amplitudes(dim, 1) {
  //set proper element of amplitudes to 1 (based on "state" parameter)
  amplitudes(state, 0) = 1;
}

QuantumState::QuantumState(const QuantumState& cp) : n(cp.n), dim(cp.dim), amplitudes(cp.amplitudes) {}

QuantumState& QuantumState::operator=(const QuantumState& cp) {
  n = cp.n;
  dim = cp.dim;
  amplitudes = cp.amplitudes;
}

int QuantumState::getN() const {
  return n;
}

matrix& QuantumState::getAmplitudes() {
  return amplitudes;
}

const matrix& QuantumState::getAmplitudes() const {
  return amplitudes;
}

ex QuantumState::getProbabilityOfState(int state) const {
  return pow(abs(amplitudes(state, 0)), 2);
}

double QuantumState::getProbabilityDoubleOfState(int state) const {
  auto p = getProbabilityOfState(state);
  if(is_a<numeric>(p)) {
    return ex_to<numeric>(p).to_double();
  }
  throw runtime_error("Invalid expression found while calculating probability.");
}

ex QuantumState::getProbabilityOfQubit(int qubit) const {
  ex sum = 0;
  int mask = QuantumState::maskQubitOn(qubit);
  for(int state = 0; state < dim; state++) {
    if(mask & state) {
      const ex& entry = amplitudes(state, 0);
      sum += pow(abs(entry), 2);
    }
  }
  return sum;
}

double QuantumState::getProbabilityDoubleOfQubit(int qubit) const {
  auto p = getProbabilityOfQubit(qubit);
  if(is_a<numeric>(p)) {
    return ex_to<numeric>(p).to_double();
  }
  throw runtime_error("Invalid expression found while calculating probability.");
}

void getProbabilities(vector<double>& receiver) const {
  for(int state = 0; state < dim; state++) {
    receiver.push_back(getProbabilityDoubleOfState(state));
  }
}

vector<double> getProbabilities() const {
  vector<double> ret;
  getProbabilities(ret);
  return ret;
}

int QuantumState::sample() const {
  double guess = get_random_number(); //generate random # in [0, 1)
  for(int choice = 0; choice < n; choice++) {
    guess -= getProbabilityDoubleOfState(choice);
    if(guess <= 0) return choice;
  }
  //invalid state
  throw runtime_error("Invalid quantum state detected during sampling.");
}

QuantumState QuantumState::measure() const {
  int state = sample();
  return QuantumState(n, state);
}

void QuantumState::measure_() {
  int state = sample();
  amplitudes = matrix(dim, 1);
  amplitudes(state, 0) = 1;
}

bool QuantumState::sample(int qubit) const {
  double guess = get_random_number();
  double p = getProbabilityDoubleOfQubit(qubit);

  return (guess < p);
}

QuantumState QuantumState::measure(int qubit) const {
  bool measurement = sample(qubit);

  vector<double> probabilities;
  getProbabilities(probabilities);

  //count states and probabilities
  ex total_probability = 0;
  for(int state = 0; state < dim; state++) {
    if(bool(mask & state) == measurement) {
      total_probability += probabilities[state];
    }
  }

  //fill in states and probabilities
  matrix& amps = matrix(dim, 1);
  for(int state = 0; state < dim; state++) {
    if(bool(mask & state) == measurement) {
      amps(state, 0) = probabilities[state] / total_probability;
    }
  }

  return QuantumState(n, amps);
}

void QuantumState::measure_(int qubit) {
  amplitudes = measure(qubit).amplitudes; //ez
}

void QuantumState::reset() {
  amplitudes = matrix(dim, 1);
  amplitudes(0, 0) = 1;
}

//TODO GS
bool QuantumState::isEntangled() const {
  QWALITY_NOT_SUPPORTED
}

QuantumState QuantumState::tensor(const QuantumState& other) const {
  return QuantumState(n + other.n, tensor_product(amplitudes, other.amplitudes));
}

void QuantumState::tensor_(const QuantumState& other) {
  amplitudes = tensor_product(amplitudes, other.amplitudes);
  n += other.n;
  dim <<= other.n;
}

//TODO GS
void QuantumState::untensor(vector<QuantumState>& receiver) const {
  QWALITY_NOT_SUPPORTED
}

vector<QuantumState> QuantumState::untensor() const {
  vector<QuantumState> ret;
  untensor(ret);
  return ret;
}

//TODO GS
QuantumState QuantumState::applyPartial(const QuantumGate& gate, const std::vector<std::size_t>& qubits) const {
  QWALITY_NOT_SUPPORTED
}

//TODO GS
void QuantumState::applyPartial_(const QuantumGate& gate, const std::vector<std::size_t>& qubits) {
  QWALITY_NOT_SUPPORTED
}

QuantumState QuantumState::applyFull(const QuantumGate& gate) const {
  return QuantumState(n, gate.getValues().mul(amplitudes));
}

void QuantumState::applyFull_(const QuantumGate& gate) {
  amplitudes = gate.getValues().mul(amplitudes);
}

} //end namespace