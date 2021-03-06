#ifndef QUANTUM_UTIL_H
#define QUANTUM_UTIL_H

#include <stdexcept>
#include <iostream>

#include <cstdio>

#include <ginac/ginac.h>

namespace Qwality {

using GiNaC::ex;
using GiNaC::matrix;

//TODO: convert all functions and data to use this type, where applicable
//also: is there a better name than index_t?
typedef unsigned index_t;

matrix conjugate_transpose(const matrix& m);

//takes two COLUMN vectors
ex inner_product(const matrix& m1, const matrix& m2);
matrix outer_product(const matrix& m1, const matrix& m2);
//takes two matrices of any size
matrix tensor_product(const matrix& m1, const matrix& m2);

double get_random_number(double min = 0, double max = 1);

std::ostream& print_binary(int state, int n, std::ostream& o);

//convert |abcd> to |dcba>
int flip_bit_order(int bits, int n);

std::ostream& print_ket(int state, int n, std::ostream& o);
std::ostream& print_bra(int state, int n, std::ostream& o);

} //end namespace

#define QWALITY_NOT_SUPPORTED throw std::runtime_error("Operation not (yet) supported.");

#endif //QUANTUM_UTIL_H
