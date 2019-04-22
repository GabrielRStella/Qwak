#include <iostream>
#include <ginac/ginac.h>
#include "quantum_state.h"
#include "quantum_gate.h"

using namespace std;
using namespace GiNaC;
using namespace Qwality;

// Functions for Deutsch problem.
bool f_0(bool x) { return false; }
bool f_1(bool x) { return x;     }
bool f_2(bool x) { return !x;    }
bool f_3(bool x) { return true;  }

int main() {
    
    cout << "----- The Deutsch Algorithm -----" << endl;

    // Prepare unitary gates.
    QuantumGate U_f0 = QuantumGate::makeUnitary(f_0);
    QuantumGate U_f1 = QuantumGate::makeUnitary(f_1);
    QuantumGate U_f2 = QuantumGate::makeUnitary(f_2);
    QuantumGate U_f3 = QuantumGate::makeUnitary(f_3);

    // Display unitary functions.
    cout << "f_0(x) = 0," << endl;
    cout << "f_1(x) = x," << endl;
    cout << "f_2(x) = !x," << endl;
    cout << "f_3(x) = 1." << endl;

    cout << endl;

    // Display unitary gates.
    cout << "U_f0 = " << U_f0 << endl;
    cout << "U_f1 = " << U_f1 << endl;
    cout << "U_f2 = " << U_f2 << endl;
    cout << "U_f3 = " << U_f3 << endl;

    cout << endl;

    // Run Deutsch on all f_i.
    for(int i = 0; i < 4; ++i) {
        // Variables for Deutsch.
        QuantumState psi(2);
        int result;

        cout << "Running Deutsch on f" << i << "..." << endl;

        // Perpare psi.
        psi.applyPartial_(QuantumGate::X, vector<int>{1});
        psi.applyFull_(QuantumGate::H.tensor(QuantumGate::H));
        switch(i) {
            case 0:
            psi.applyFull_(U_f0);
            break;
            case 1:
            psi.applyFull_(U_f1);
            break;
            case 2:
            psi.applyFull_(U_f2);
            break;
            case 3: 
            psi.applyFull_(U_f3);
            break;
        }
        psi.applyPartial_(QuantumGate::H, vector<int>{0});
        result = psi.sample(0);
    
        // Display results
        cout << "Result of measuring is " << result << endl;
    }

    return 0;

}
