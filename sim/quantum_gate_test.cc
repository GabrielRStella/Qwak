#include <iostream>
#include <ginac/ginac.h>
#include "quantum_gate.h"

using namespace std;
using namespace GiNaC;
using namespace Qwality;

int main() {
    
    vector<int> swap_i = {1,0,2};
    
    cout << QuantumGate::I << endl;
    cout << QuantumGate::X << endl;
    cout << QuantumGate::Y << endl;
    cout << QuantumGate::Z << endl;
    cout << QuantumGate::H << endl;
    cout << (QuantumGate::R(2)).inverse() << endl;
    cout << QuantumGate::control(QuantumGate::H) << endl;
    cout << QuantumGate::swap(swap_i) << endl;

    return 0;

}
