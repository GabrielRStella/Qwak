#include "quantum_state.h"

#include <iostream>

using namespace Qwality;

/*
TODO:
-output operators?
-"circuit" class that combines states + gates and results in a mixture of qubit states + classical bits
*/

int main() {
  QuantumState qs;
  QuantumGate f = H();

  std::cout << "Hi!" << std::endl;

  std::cout << qs.getAmplitudes() << std::endl;
  std::cout << f.getValues() << std::endl;

  QuantumState qs2 = qs.applyFull(f);
  std::cout << qs2.getAmplitudes() << std::endl;

  //qs.applyFull_(f);

  std::cout << QuantumState(1, 1).applyFull(f).getAmplitudes() << std::endl;
  std::cout << QuantumState(1, 1).applyFull(f).applyFull(f).getAmplitudes() << std::endl;

  return 0;
}