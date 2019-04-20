#include "qwak.h"

#include <iostream>

using namespace Qwak;

/*
TODO:
*/

using namespace std;

void execute(QwakParser& parser, Program& p, Environment& e, const std::string& stmt) {
  Object o = parser.execute(stmt, p, e);

  cout << "Executed statement: \"" << stmt << "\"" << endl;

  cout << "Result: " << o << endl;
}

int main() {

  QwakParser parser;

  cout << "Created parser..." << endl;
  cout << "MOTD: " << parser.motd() << endl;

  Program* pr = parser.createEmptyProgram();
  Program& p = *pr;

  cout << "Created program..." << endl;

  Environment e;

  cout << "Created environment..." << endl << endl;

  execute(parser, p, e, "l = H");
  cout << endl;
  execute(parser, p, e, "l");
  cout << endl;

  cout << "State: " << e.getState() << endl << endl;

  execute(parser, p, e, "a=|0>");
  cout << endl;

  cout << "State: " << e.getState() << endl << endl;

  execute(parser, p, e, "b=|0>");
  cout << endl;

  cout << "State: " << e.getState() << endl << endl;

  execute(parser, p, e, "l(a)");

  cout << "State: " << e.getState() << endl << endl;

  execute(parser, p, e, "H(a)");

  cout << "State: " << e.getState() << endl << endl;

  execute(parser, p, e, "H(a)");
  execute(parser, p, e, "H(b)");

  cout << "State: " << e.getState() << endl << endl;

  cout << endl << "Bye!" << endl;

  return 0;
}