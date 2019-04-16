#ifndef QWAK_H
#define QWAK_H

#include "parse.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Qwak {

using std::string;
using std::vector;
using std::unordered_map;

//a statement of some sort that goes in a function body
class Statement {
private:

public:
  bool parse(TokenStream& tokens);
  //TODO: API
};

//the declaration of a function, e.g. "function example(U, x)"
class FunctionPrototype {
private:

public:
  bool parse(TokenStream& tokens);
  //TODO: API
  string getName();
};

//a function and its body
class Function {
private:
  FunctionPrototype proto;
  vector<Statement> statements;
public:
  bool parse(TokenStream& tokens);
  //TODO: API
  string getName();
};

class Program {
private:
  vector<Function> functions;
  unordered_map<string, Function*> functionsByName;
public:
  bool parse(TokenStream& tokens);
  //TODO: API
  Function& getFunction(string name);
};

class QwakParser {
private:

public:
  QwakParser();

  //parses string -> Tokens -> AST
  bool parse(const std::string& buffer, TokenTree& root);
  //parses AST -> program
  //(TODO)
  
};

} //end namespace

#endif //QWAK_H