#include "quantum_state.h"

#include <iostream>

using namespace Qwality;

/*
TODO:
-"circuit" class that combines states + gates and results in a mixture of qubit states + classical bits
*/

int main() {
  QuantumState qs(5);
  QuantumGate h = H();

  std::cout << "Hi!" << std::endl;

  QuantumState qs2 = qs.applyFull(h);
  qs.applyFull_(h);

  return 0;
}