#include "quantum_state.h"

#include <iostream>

using namespace Qwality;

/*
TODO:
-output operators
-"circuit" class that combines states + gates and results in a mixture of qubit states + classical bits
*/

using namespace std;

int main() {

  cout << "Hi!" << endl;

  {
    QuantumState qs(2);
    cout << "qs: " << qs << endl;
    cout << "H(qs[0]): " << qs.applyPartial(QuantumGate::H, {0}) << endl;
  }

  {
    QuantumState qs;
    cout << "qs: " << qs << endl;
    cout << "H(qs): " << qs.applyFull(QuantumGate::H) << endl;
  }

  {
    QuantumState qs(2);
    matrix mH({{1, 1}, {1, -1}});
    QuantumGate HH(2, tensor_product(mH, mH));
    cout << "qs: " << qs << endl;
    cout << "HH(qs): " << qs.applyFull(HH) << endl;
    cout << "H(qs[0]): " << qs.applyPartial(QuantumGate::H, {0}) << endl;
  }

  {
    QuantumState qs1;
    QuantumState qs2(2, 1);
    QuantumState qs3(1, 1);
    //qs3.applyFull_(H());

    cout << "qs1: " << qs1 << endl;
    cout << "qs2: " << qs2 << endl;
    cout << "qs3: " << qs3 << endl;
    cout << "qs1 ** qs2: " << qs1.tensor(qs2) << endl;
    cout << "qs2 ** qs1: " << qs2.tensor(qs1) << endl;
    cout << "qs1 ** qs3: " << qs1.tensor(qs3) << endl;
    cout << "qs1 ** qs3 ** qs2: " << qs1.tensor(qs3).tensor(qs2) << endl;
  }

  return 0;
}