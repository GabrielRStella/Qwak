#include "util.h"

namespace Qwality {

matrix conjugate_transpose(const matrix& m) {
  matrix tr = m.transpose();
  for(unsigned i = 0; i < tr.rows(); i++) {
    for(unsigned j = 0; j < tr.cols(); j++) {
      tr(i, j) = tr(i, j).conjugate();
    }
  }
  return tr;
}

ex inner_product(const matrix& m1, const matrix& m2) {
  return conjugate_transpose(m1).mul(m2)(0, 0);
}

matrix outer_product(const matrix& m1, const matrix& m2) {
  return m1.mul(conjugate_transpose(m2));
}

matrix tensor_product(const matrix& m1, const matrix& m2) {
  int ro1 = m1.rows();
  int ro2 = m2.rows();
  int co1 = m1.cols();
  int co2 = m2.cols();

  int rows = ro1 * ro2;
  int cols = co1 * co2;

  matrix result = matrix(rows, cols);
  for(int row = 0; row < rows; row++) {
    for(int col = 0; col < cols; col++) {
      int row_m1 = row / ro2;
      int col_m1 = col / co2;
      int row_m2 = row % ro2;
      int col_m2 = col % co2;
      result(row, col) = m1(row_m1, col_m1) * m2(row_m2, col_m2);
    }
  }

  return result;
}

#include <random>

double get_random_number(double min, double max) {
  static std::uniform_real_distribution<double> distr; // [0, 1)
  static std::mt19937_64 mt;
  double val = distr(mt);
  return val * (max - min) + min;
}

}