#include "quantum_state.h"

#include "util.h"

namespace Qwality {

using std::runtime_error;
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
  p = p.evalf();
  if(is_a<numeric>(p)) {
    return ex_to<numeric>(p).to_double();
  }
  throw runtime_error("Invalid expression found while calculating probability.");
}

ex QuantumState::getProbabilityOfQubit(int qubit) const {
  qubit = n - qubit - 1; //flip
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
  p = p.evalf();
  if(is_a<numeric>(p)) {
    return ex_to<numeric>(p).to_double();
  }
  throw runtime_error("Invalid expression found while calculating probability.");
}

void QuantumState::getProbabilities(vector<ex>& receiver) const {
  for(int state = 0; state < dim; state++) {
    receiver.push_back(getProbabilityOfState(state));
  }
}

vector<ex> QuantumState::getProbabilities() const {
  vector<ex> ret;
  getProbabilities(ret);
  return ret;
}

void QuantumState::getProbabilityDoubles(vector<double>& receiver) const {
  for(int state = 0; state < dim; state++) {
    receiver.push_back(getProbabilityDoubleOfState(state));
  }
}

vector<double> QuantumState::getProbabilityDoubles() const {
  vector<double> ret;
  getProbabilityDoubles(ret);
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

QuantumState QuantumState::measure(int qubit, bool* choice) const {
  bool measurement = sample(qubit);
  if(choice) *choice = measurement;

  qubit = n - qubit - 1;
  int mask = QuantumState::maskQubitOn(qubit);

  vector<ex> probabilities;
  getProbabilities(probabilities);

  //count states and probabilities
  ex total_probability = 0;
  for(int state = 0; state < dim; state++) {
    if(bool(mask & state) == measurement) {
      total_probability += probabilities[state];
    }
  }

  //fill in states and probabilities
  matrix amps = matrix(dim, 1);
  for(int state = 0; state < dim; state++) {
    if(bool(mask & state) == measurement) {
      amps(state, 0) = sqrt(probabilities[state] / total_probability);
    }
  }

  return QuantumState(n, amps);
}

void QuantumState::measure_(int qubit, bool* choice) {
  amplitudes = measure(qubit, choice).amplitudes; //ez
}

QuantumState QuantumState::measure(const vector<int>& qubits, int* choice) const {
  QuantumState tmp(*this);

  int theChoice = 0;

  int idx = 0;
  for(int qubit : qubits) {
    bool thisChoice;
    tmp.measure_(qubit, &thisChoice);
    theChoice |= (int(thisChoice) << idx);
    idx++;
  }

  *choice = flip_bit_order(theChoice, n);
  return tmp;
}

void QuantumState::measure_(const vector<int>& qubits, int* choice) {
  amplitudes = measure(qubits, choice).amplitudes; //ez
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

QuantumState QuantumState::applyPartial(const QuantumGate& gate, const vector<int>& qubits_) const {
  int len = qubits_.size();
  int dim2 = 1 << len;

  //adjust to local positioning
  vector<int> qubits(len);
  for(int q = 0; q < len; q++) {
    qubits[q] = n - qubits_[q] - 1;
  }

  //construct a mask for removing the partial state
  int mask = 0;
  for(auto qubit : qubits) {
    mask |= (1 << qubit);
  }
  mask = ~mask;

  matrix result(dim, 1);

  const matrix& values = gate.getValues();

  for(int state = 0; state < dim; state++) {
    const ex& scaleFromThis = amplitudes(state, 0);
    if(scaleFromThis.is_zero()) continue;
    //construct partial state
    int partialState = 0;
    for(int qubit = 0; qubit < len; qubit++) {
      if(state & (1 << qubits[qubit])) partialState |= (1 << qubit);
    }
    //apply gate
    for(int newPartialState = 0; newPartialState < dim2; newPartialState++) {
      const ex& scaleFromGate = values(partialState, newPartialState);
      if(scaleFromGate.is_zero()) continue;
      int newState = state & mask;
      for(int qubit = 0; qubit < len; qubit++) {
        if(newPartialState & (1 << qubit)) newState |= (1 << qubits[qubit]);
      }
      result(newState, 0) += scaleFromThis * scaleFromGate;
    }
  }

  return QuantumState(n, result);
}

void QuantumState::applyPartial_(const QuantumGate& gate, const vector<int>& qubits) {
  amplitudes = applyPartial(gate, qubits).amplitudes; //aha, inefficient but SIMPLE
}

QuantumState QuantumState::applyFull(const QuantumGate& gate) const {
  return QuantumState(n, gate.getValues().mul(amplitudes));
}

void QuantumState::applyFull_(const QuantumGate& gate) {
  amplitudes = gate.getValues().mul(amplitudes);
}

} //end namespace

std::ostream& operator<<(std::ostream& o, const Qwality::QuantumState& qs) {
  bool printed = false;
  for(int state = 0; state < qs.dim; state++) {

    const GiNaC::ex& amp = qs.amplitudes(state, 0);
    if(!amp.is_zero()) {
      if(printed) o << "+";

      if(!amp.is_equal(GiNaC::ex(1))) o << amp;
      Qwality::print_ket(state, qs.n, o);
      printed = true;
    }
  }
  return o;
}
