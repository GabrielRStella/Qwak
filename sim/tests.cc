#include "quantum_state.h"

#include <iostream>

using namespace Qwality;

/*
TODO:
-output operators?
-"circuit" class that combines states + gates and results in a mixture of qubit states + classical bits
*/

/*
also note: when subscripting QuantumState, qubits are referenced in flipped notation.
i.e., |0010> = 0b0100 = 4
*/

using namespace std;

int main() {
  QuantumGate f = H();

  cout << "Hi!" << endl;

  {
    QuantumState qs;
    cout << "f: " << f.getValues() << endl;
    cout << qs.getAmplitudes() << endl;
    QuantumState qs2 = qs.applyFull(f);
    cout << qs2.getAmplitudes() << endl;
    cout << QuantumState(1, 1).applyFull(f).getAmplitudes() << endl;
    cout << QuantumState(1, 1).applyFull(f).applyFull(f).getAmplitudes() << endl;
  }

  {
    QuantumState qs(2);
    cout << "f: " << f.getValues() << endl;
    cout << qs.getAmplitudes() << endl;
    cout << qs.applyPartial(f, {0}).getAmplitudes() << endl;
  }

  return 0;
}