#include "qwak.h"

namespace Qwak {

//helpers

template<typename T>
void parseSome(TokenStream& tokens, vector<T>& store) {
  T tmp;

  int pos = tokens.getPos();
  while(tmp.parse(tokens)) {
    store.push_back(tmp);
    pos = tokens.getPos();
  }
  tokens.setPos(pos);
}

//semantic elements

//TODO
bool Statement::parse(TokenStream& tokens) {
}

//TODO
bool FunctionPrototype::parse(TokenStream& tokens) {
}

//TODO
string FunctionPrototype::getName() {
}

bool Function::parse(TokenStream& tokens) {
  int pos = tokens.getPos();

  if(proto.parse(tokens)) {
    parseSome<Statement>(tokens, statements);
    return true;
  } else {
    tokens.setPos(pos);
    return false;
  }
}

//TODO
string Function::getName() {
}

bool Program::parse(TokenStream& tokens) {
  parseSome<Function>(tokens, functions);
  for(Function& f: functions) {
    functionsByName[f.getName()] = &f;
  }

  return true;
}

} //end namespace