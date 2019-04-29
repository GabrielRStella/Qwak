#include <iostream>
#include <ginac/ginac.h>
using namespace std;
using namespace GiNaC;

matrix conjugate_transpose(const matrix& m) {
  matrix tr = m.transpose();
  for(unsigned i = 0; i < tr.rows(); i++) {
    for(unsigned j = 0; j < tr.cols(); j++) {
      tr(i, j) = tr(i, j).conjugate();
    }
  }
  return tr;
}

void print(const matrix& m) {
  for(int r = 0; r < m.rows(); r++) {
    for(int c = 0; c < m.cols(); c++) {
      cout << m(r, c) << " ";
    }
    cout << endl;
  }
}

int main()
{

  {
    //gate
    symbol a("a");
    symbol b("b");
    symbol c("c");
    symbol d("d");
    symbol e("e");
    symbol f("f");
    symbol g("g");
    symbol h("h");
    //state
    symbol x("x");
    symbol y("y");
    symbol z("z");
    symbol w("w");

    matrix op(4, 2);
    op(0, 0) = a;
    op(0, 1) = b;
    op(1, 0) = c;
    op(1, 1) = d;
    op(2, 0) = e;
    op(2, 1) = f;
    op(3, 0) = g;
    op(3, 1) = h;
    matrix op2 = conjugate_transpose(op);
    matrix state(2, 2);
    state(0, 0) = x;
    state(0, 1) = y;
    state(1, 0) = z;
    state(1, 1) = w;

    cout << endl << op << endl;
    cout << endl << state << endl;

    matrix result = op.mul(state).mul(op2);

    print(result);

    cout << endl << result << endl;
  }

  {
    symbol x("x");
    symbol y("y");
    symbol z("z");
    symbol w("w");

    matrix op(4, 2);
    op(0, 0) = 1;
    op(0, 1) = 0;
    op(1, 0) = 0;
    op(1, 1) = 1;
    op(2, 0) = 1;
    op(2, 1) = 0;
    op(3, 0) = 0;
    op(3, 1) = 1;
    matrix op2 = conjugate_transpose(op);
    matrix state(2, 2);
    state(0, 0) = x;
    state(0, 1) = y;
    state(1, 0) = z;
    state(1, 1) = w;

    cout << endl << op << endl;
    cout << endl << state << endl;

    matrix result = op.mul(state).mul(op2);
    matrix opmul = op2.mul(op); //for ensuring channel sums to I

    //cout << endl << result << endl;
    //cout << endl << opmul << endl;

cout << "result:" << endl;
    print(result);
    cout << endl;
cout << "opmul:" << endl;
    print(opmul);
  }

/*
    symbol x("x"), y("y");
    ex poly;

    for (int i=0; i<3; ++i)
        poly += factorial(i+16)*pow(x,i)*pow(y,2-i);

    cout << poly << endl;
*/
    return 0;
}