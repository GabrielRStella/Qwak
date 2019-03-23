#ifndef QUANTUM_UTIL_H
#define QUANTUM_UTIL_H

#include <ginac/ginac.h>

namespace Qwality {

using GiNaC::ex;
using GiNaC::matrix;

ex inner_product(const matrix& m1, const matrix& m2);
matrix outer_product(const matrix& m1, const matrix& m2);
matrix tensor_product(const matrix& m1, const matrix& m2);

}

#endif //QUANTUM_UTIL_H