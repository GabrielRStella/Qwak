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
  virtual ~Function();
  //API
  virtual const string getName() const = 0;
  virtual const vector<string>& getArgs() const = 0;
  //substate is an additional argument for gates and/or built-in functions
  virtual Object execute(Environment& e, Program& p, const vector<int>& substate) const = 0;
};

class Program {
protected:
  vector<Function*> functions;
  unordered_map<string, Function*> functionsByName;
public:
  void addFunction(Function* f);
  bool tryAddFunction(Function* f); //try to add, but don't replace

  Function* getFunction(string name) const;
  const vector<Function*>& getFunctions() const;

  void addBuiltinFunctions();
};

class QwakParser {
private:
  vector<TokenRule*> tokenRules;
  vector<std::string> messages;
  std::string version_;
public:
  QwakParser();

  const std::string& version();
  //message of the day
  const std::string& motd();

  Program* createEmptyProgram();

  //parses string -> Tokens -> AST -> Program
  Program* parse(const std::string& buffer);

  Object execute(const std::string& statement, Program& p, Environment& e);
};

class ProgramError : public QwakError {
public:
  ProgramError(const string& what);
};

} //end namespace

#endif //QWAK_H