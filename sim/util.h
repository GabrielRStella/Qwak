#ifndef QUANTUM_UTIL_H
#define QUANTUM_UTIL_H

#include <stdexcept>

#include <ginac/ginac.h>

namespace Qwality {

using GiNaC::ex;
using GiNaC::matrix;

using std::runtime_error;

ex inner_product(const matrix& m1, const matrix& m2);
matrix outer_product(const matrix& m1, const matrix& m2);
matrix tensor_product(const matrix& m1, const matrix& m2);

double get_random_number(double min = 0, double max = 1);

} //end namespace

#define QWALITY_NOT_SUPPORTED throw runtime_error("Operation not (yet) supported.");

#endif //QUANTUM_UTIL_H