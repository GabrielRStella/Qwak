#ifndef QWAK_H
#define QWAK_H

#include "parse.h"

#include "env.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Qwak {

using std::string;
using std::vector;
using std::unordered_map;

class Program;

//an executable function
class Function {
public:
  //TODO: API
  virtual const string getName() const = 0;
  virtual const vector<string>& getArgs() const;
  virtual Object execute(Environment& e, Program& p) const = 0;
};

class Program {
protected:
  vector<Function*> functions;
  unordered_map<string, Function*> functionsByName;
public:
  void addFunction(Function* f);

  const Function* getFunction(string name) const;
  const vector<Function*>& getFunctions() const;
};

class QwakParser {
private:
  vector<TokenRule*> tokenRules;
public:
  QwakParser();

  //parses string -> Tokens -> AST -> Program
  Program* parse(const std::string& buffer);
};

class ProgramError : public QwakError {
public:
  ParserError(const string& what);
};

} //end namespace

#endif //QWAK_H