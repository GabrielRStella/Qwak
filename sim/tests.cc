#include "quantum_state.h"

#include <iostream>

using namespace Qwality;

int main() {
  QuantumState qs(5);
  QuantumGate h = H();

  std::cout << "Hi!" << std::endl;

  QuantumState qs2 = qs.applyFull(h);
  qs.applyFull_(h);

  return 0;
}