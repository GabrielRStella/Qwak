#include <iostream>
#include <ginac/ginac.h>
#include "quantum_state.h"
#include "quantum_gate.h"
using namespace std;
using namespace GiNaC;
using namespace Qwality;

int main() {
    int n = 2;
    bool x[] = {1,0,1}; // x = {x_1,x_2,x_3}.

    ex omega = 0;
    for(int i = 0; i < n; ++i) {
        omega += x[i] * GiNaC::pow(2,-(i+1));
    }

    cout << "omega: " << omega << endl;

    matrix mat((1<<n),1);
    for(int i = 0; i < (1<<n); ++i) {
        mat(i,0) = pow(Euler,2*Pi*I*omega*ex(i));
    }

    QuantumState psi(n,mat);
    
    cout << "|psi_0>: " << psi << endl;

    psi.applyPartial_(QuantumGate::H, vector<int>{1});
    psi.measure_(1);

    cout << "|psi_1>: " << psi << endl;

    return 0;
}
