#include "qwak.h"

#include <iostream>

using namespace Qwak;

/*
TODO:
*/

using namespace std;

int main() {

  QwakParser parser;

  cout << "Hi!" << endl;

  Program* pr = parser.parse(""); //empty program
  Program& p = *pr;

  cout << "Hey!" << endl;

  Environment e;

  cout << "Hello!" << endl;

  std::string stmt = "l = H";
  Object o = parser.execute(stmt, p, e);

  cout << "Heyo!" << endl;

  cout << o << endl;

  return 0;
}