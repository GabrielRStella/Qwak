#include "quantum_gate.h"

using namespace GiNaC;

namespace Qwality {

QuantumGate::QuantumGate(int n_qubits) : n(n_qubits), dim(1 << n), values(dim, dim) {
    //have to make identity matrix here
    //because matrix(r, c) generates 0-matrix
    for(int i = 0; i < dim; i++) {
        values(i, i) = 1;
    }
}

QuantumGate::QuantumGate(int n_qubits, const matrix& values_) : n(n_qubits), dim(1 << n), values(values_) {}

int QuantumGate::getN() const {
    return n;
}

const matrix& QuantumGate::getValues() const {
    return values;
}

matrix& QuantumGate::getValues() {
    return values;
}

QuantumGate QuantumGate::tensor(const QuantumGate& other) const {
    return QuantumGate(n + other.n, tensor_product(values, other.values));
}

void QuantumGate::tensor_(const QuantumGate& other) {
    values = tensor_product(values, other.values);
    n += other.n;
    dim <<= other.n;
}

QuantumGate QuantumGate::andThen(const QuantumGate& next) const {
    return QuantumGate(n, next.values.mul(values));
}

void QuantumGate::andThen_(const QuantumGate& next) {
    values = next.values.mul(values);
}

QuantumGate QuantumGate::conjugate() const {
    return QuantumGate(n, conjugate_transpose(values));
}

void QuantumGate::conjugate_() {
    values = conjugate_transpose(values);
}

matrix H_mat = {{1,1},{1,-1}};
matrix X_mat = {{0,1},{1,0}};
matrix Y_mat = {{0,-I},{I,0}};
matrix Z_mat = {{1,0},{0,-1}};

const QuantumGate QuantumGate::I(1);
const QuantumGate QuantumGate::H(1, H_mat.mul_scalar(ex(1)/sqrt(ex(2))));
const QuantumGate QuantumGate::X(1, X_mat);
const QuantumGate QuantumGate::Y(1, Y_mat);
const QuantumGate QuantumGate::Z(1, Z_mat);

QuantumGate QuantumGate::R(int k) {
    matrix R_k = {{1,0},{0,pow(Euler,(2*Pi*GiNaC::I/(pow(2,k))))}};
    return QuantumGate(1, R_k);
}

QuantumGate QuantumGate::control(const QuantumGate& gate) {
    QuantumGate result(gate.n + 1);
    for(int i = 0; i < gate.dim; ++i) {
        for(int j = 0; j < gate.dim; ++j) {
            result.values(j + gate.dim, i + gate.dim) = gate.values(j,i);
        }
    }
    return result;
}

QuantumGate QuantumGate::swap(vector<int>& indices) {
    int n = indices.size();
    matrix mat(1<<n,1<<n);

    for(int i = 0; i < (1<<indices.size()); ++i) {
        // We want to develop a matrix of the form |j><i|where
        // every i is a bit strings and each corresponding j
        // is the same bitstring with the bits swapped as
        // specified in indices.
        int j = 0;
        for(int k = 0; k < n; ++k) {
            j |= ((i >> indices[k]) & 1) << k;
        }
        mat(j,i) = 1;
    }

    QuantumGate result(n, mat);

    return result;
}

} //end namespace

std::ostream& operator<<(std::ostream& o, const Qwality::QuantumGate& qg) {
    bool printed = false;
    for(int i = 0; i < qg.dim; ++i) {
        for(int j = 0; j < qg.dim; ++j) {

            const GiNaC::ex& val = qg.values(i, j);
            if(!val.is_zero()) {
                if(!val.compare(1)) {
                    if(printed) o << "+";
                } else if(!val.compare(-1)) {
                    o << "-";
                } else {
                    if(printed) o << "+";
                    o << "(" << val << ")";
                }
                Qwality::print_ket(i, qg.n, o);
                Qwality::print_bra(j, qg.n, o);
                printed = true;
            }
        }
    }
    return o;
}


