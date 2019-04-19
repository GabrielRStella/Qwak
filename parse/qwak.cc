#include "qwak.h"

//https://stackoverflow.com/q/13445688
#include <cstdlib>
#include <ctime>

//TODO: |0^n> (currently only supports |binary> without ^tensor)

#include <initializer_list>

namespace Qwak {

//just start at 100 for safety

//symbols
//parens
const int TOKEN_TYPE_PARENS_LEFT = 100;
const int TOKEN_TYPE_PARENS_RIGHT = 101;
//square bracket
const int TOKEN_TYPE_BRACKET_LEFT = 102;
const int TOKEN_TYPE_BRACKET_RIGHT = 103;
//curly braces
const int TOKEN_TYPE_BRACE_LEFT = 104;
const int TOKEN_TYPE_BRACE_RIGHT = 105;
//bra-kets
const int TOKEN_TYPE_ANGLE_LEFT = 106; // <
const int TOKEN_TYPE_ANGLE_RIGHT = 107; // >
//misc symbols
const int TOKEN_TYPE_COMMA = 110; // , for args or lists
const int TOKEN_TYPE_DOTS = 111; // .. for ranges
const int TOKEN_TYPE_PIPE = 112; // |
const int TOKEN_TYPE_ASSIGN = 113; // =
const int TOKEN_TYPE_EQUALS = 114; // ==
//ops
const int TOKEN_TYPE_PLUS = 120;
const int TOKEN_TYPE_MINUS = 121;
const int TOKEN_TYPE_CONCAT = 122; // ^ for |0^n> etc
const int TOKEN_TYPE_EXPONENT = 123; // **
const int TOKEN_TYPE_TENSOR = 124; // ^*
const int TOKEN_TYPE_TENSOR_EXPONENT = 125; // ^**
//keywords
const int TOKEN_TYPE_KEYWORD_FUNCTION = 200;
const int TOKEN_TYPE_KEYWORD_RETURN = 201;
//other
const int TOKEN_TYPE_LITERAL = 300; //an integer literal, possibly in |binary> notation
const int TOKEN_TYPE_IDENTIFIER = 302; //identifier, e.g. a variable or function name

//helpers

template<typename T>
void parseSome(TokenStream& tokens, vector<T*>& store) {
  T* tmp;

  int pos = tokens.getPos();
  while(true) {
    tmp = new T();
    if(!tmp->parse(tokens)) {
      delete tmp;
      break;
    }
    store.push_back(tmp);
    pos = tokens.getPos();
  }
  tokens.setPos(pos);
}

void parseSeparated(TokenStream& tokens, int type, int separator, vector<string>& store) {
  Token tmp;

  int pos = tokens.getPos();

  if(tokens(type, &tmp)) {
    store.push_back(tmp.getValue());
    while(true) {
      pos = tokens.getPos();
      if(tokens(separator)) {
        if(tokens(type, &tmp)) {
          store.push_back(tmp.getValue());
        } else {
          throw ParserError("Expected value after comma", tokens.getPos());
        }
      } else {
        tokens.setPos(pos);
        break;
      }
    }
  } else {
    //not strictly necessary, but whatever
    tokens.setPos(pos);
  }
}

template<typename T>
void parseList(TokenStream& tokens, int separator, vector<T*>& store) {
  int pos = tokens.getPos();

  T* t = new T();
  if(t->parse(tokens)) {
    store.push_back(t);
    t = new T();
    while(true) {
      pos = tokens.getPos();
      if(tokens(separator)) {
        if(t->parse(tokens)) {
          store.push_back(t);
          t = new T();
        } else {
          delete t;
          throw ParserError("Expected value after comma", tokens.getPos());
        }
      } else {
        tokens.setPos(pos);
        delete t;
        break;
      }
    }
  } else {
    delete t;
    tokens.setPos(pos);
  }
}

template<typename T>
bool parseListBounded(TokenStream& tokens, int separator, vector<T*>& store, int left = TOKEN_TYPE_PARENS_LEFT, int right = TOKEN_TYPE_PARENS_RIGHT) {
  Token tmp;

  int pos = tokens.getPos();

  if(tokens(left)) {
    //parse some
    parseList(tokens, separator, store);
    if(tokens(right)) {
      return true;
    }
  }
  tokens.setPos(pos);
  return false;
}

//semantic elements

//supertype for expressions
class ExpressionType {
public:
  virtual bool parse(TokenStream& tokens) = 0;
  virtual Object evaluate(Environment& e, Program& p) const = 0;
};

class ExpressionTypeLiteralStateAST : public ExpressionType {
  int state;
  int n;
public:
  virtual bool parse(TokenStream& tokens) override {
    int pos = tokens.getPos();
    Token tmp;
    if(tokens(TOKEN_TYPE_PIPE) && tokens(TOKEN_TYPE_LITERAL, &tmp) && tokens(TOKEN_TYPE_ANGLE_RIGHT)) {
      state = std::stoi(tmp.getValue(), nullptr, 2);
      n = tmp.getValue().size(); //number of qubits = length of binary string
      return true;
    }
    tokens.setPos(pos);
    return false;
  }

  virtual Object evaluate(Environment& e, Program& p) const override {
    return e.createObject(Qwality::QuantumState(n, state));
  }
};

class ExpressionTypeLiteralIntAST : public ExpressionType {
  int value;
public:
  virtual bool parse(TokenStream& tokens) override {
    Token tmp;
    if(tokens(TOKEN_TYPE_LITERAL, &tmp)) {
      value = std::stoi(tmp.getValue());
      return true;
    }
    return false;
  }

  virtual Object evaluate(Environment& e, Program& p) const override {
    return e.createObject(value);
  }
};

class ExpressionTypeIdentifierAST : public ExpressionType {
  string name;
public:
  virtual bool parse(TokenStream& tokens) override {
    Token tmp;
    if(tokens(TOKEN_TYPE_IDENTIFIER, &tmp)) {
      name = tmp.getValue();
      return true;
    }
    return false;
  }

  virtual Object evaluate(Environment& e, Program& p) const override {
    return e[name];
  }
};

class ExpressionTypeSubstateAST : public ExpressionType {
  string name;
  vector<int> qubits;
public:
  virtual bool parse(TokenStream& tokens) override {
    Token tmp;
    int pos = tokens.getPos();
    if(tokens(TOKEN_TYPE_IDENTIFIER, &tmp) && tokens(TOKEN_TYPE_BRACKET_LEFT)) {
      name = tmp.getValue();
      vector<string> qubitsTmp;
      parseSeparated(tokens, TOKEN_TYPE_LITERAL, TOKEN_TYPE_COMMA, qubitsTmp);
      if(TOKEN_TYPE_BRACKET_RIGHT) {
        for(const string& s : qubitsTmp) qubits.push_back(std::stoi(s));
        return true;
      } else {
        throw ParserError("Invalid bracket sequence after identifier", tokens.getPos());
      }
      return true;
    }
    tokens.setPos(pos);
    return false;
  }

  virtual Object evaluate(Environment& e, Program& p) const override {
    Object o = e[name];
    if(o.getType() == DATATYPE_STATE) {
      vector<int>& qubits_ = o.castData<vector<int>>();
      vector<int>* newQubits = new vector<int>();
      for(int qubit : qubits) newQubits->push_back(qubits_[qubit]);
      return Object(DATATYPE_STATE, newQubits);
    } else {
      return OBJECT_NONE; //ehhh
    }
  }
};

//literal or variable
//literal = <decimal number> or |<binary number>>
//variable = <identifier>
class ExpressionTypePrimaryAST : public ExpressionType {
  ExpressionType* expression;
public:
  virtual bool parse(TokenStream& tokens) override {
    int pos = tokens.getPos();

    expression = new ExpressionTypeLiteralStateAST();
    if(expression->parse(tokens)) {
      return true;
    } else {
      delete expression;
      tokens.setPos(pos);
      expression = new ExpressionTypeLiteralIntAST();
      if(expression->parse(tokens)) {
        return true;
      } else {
        delete expression;
        tokens.setPos(pos);
        expression = new ExpressionTypeSubstateAST();
        if(expression->parse(tokens)) {
          return true;
        } else {
          delete expression;
          tokens.setPos(pos);
          expression = new ExpressionTypeIdentifierAST();
          if(expression->parse(tokens)) {
            return true;
          } else {
            delete expression;
            tokens.setPos(pos);
            return false;
          }
        }
      }
    }
  }

  virtual Object evaluate(Environment& e, Program& p) const override {
    return expression ? expression->evaluate(e, p) : OBJECT_NONE;
  }
};

//expr ** ^* ^**
class ExpressionTypeSecondaryAST : public ExpressionType {
  ExpressionTypePrimaryAST left;
  ExpressionTypePrimaryAST right;
  string op;
  bool hasRight;
public:
  virtual bool parse(TokenStream& tokens) override {
    int pos = tokens.getPos();
    if(left.parse(tokens)) {
      pos = tokens.getPos();
      Token opt;
      if(tokens(TOKEN_TYPE_EXPONENT, &opt) || tokens(TOKEN_TYPE_TENSOR, &opt) || tokens(TOKEN_TYPE_TENSOR_EXPONENT, &opt)) {
        op = opt.getValue();
        if(right.parse(tokens)) {
          hasRight = true;
          return true;
        } else {
          throw ParserError("Expected primary expression after operator " + op, tokens.getPos());
        }
      } else {
        hasRight = false;
        tokens.setPos(pos);
        return true;
      }
    }
    tokens.setPos(pos);
    return false;
  }

  virtual Object evaluate(Environment& e, Program& p) const override {
    if(hasRight) {
      Object o1 = left.evaluate(e, p);
      Object o2 = right.evaluate(e, p);
      return e.applyOperator(op, o1, o2);
    } else {
      return left.evaluate(e, p);
    }
  }
};

//expr + -
class ExpressionTypeTertiaryAST : public ExpressionType {
  ExpressionTypeSecondaryAST left;
  ExpressionTypeSecondaryAST right;
  string op;
  bool hasRight;
public:
  virtual bool parse(TokenStream& tokens) override {
    int pos = tokens.getPos();
    if(left.parse(tokens)) {
      pos = tokens.getPos();
      Token opt;
      if(tokens(TOKEN_TYPE_PLUS, &opt) || tokens(TOKEN_TYPE_MINUS, &opt)) {
        op = opt.getValue();
        if(right.parse(tokens)) {
          hasRight = true;
          return true;
        } else {
          throw ParserError("Expected seconday expression after operator " + op, tokens.getPos());
        }
      } else {
        hasRight = false;
        tokens.setPos(pos);
        return true;
      }
    }
    tokens.setPos(pos);
    return false;
  }

  virtual Object evaluate(Environment& e, Program& p) const override {
    if(hasRight) {
      Object o1 = left.evaluate(e, p);
      Object o2 = right.evaluate(e, p);
      return e.applyOperator(op, o1, o2);
    } else {
      return left.evaluate(e, p);
    }
  }
};

//expr a or (a)
class ExpressionTypeQuaternaryAST : public ExpressionType {
  ExpressionTypeTertiaryAST expr;
public:
  virtual bool parse(TokenStream& tokens) override {
    int pos = tokens.getPos();
    if(tokens(TOKEN_TYPE_PARENS_LEFT)) {
      if(expr.parse(tokens) && tokens(TOKEN_TYPE_PARENS_RIGHT)) {
        return true;
      } else {
        tokens.setPos(pos);
        return false;
      }
    } else {
      return expr.parse(tokens);
    }
  }

  virtual Object evaluate(Environment& e, Program& p) const override {
    return expr.evaluate(e, p);
  }
};

//expr a == b
class ExpressionTypeQuinaryAST : public ExpressionType {
  ExpressionTypeQuaternaryAST left;
  ExpressionTypeQuaternaryAST right;
  string op;
  bool hasRight;
public:
  virtual bool parse(TokenStream& tokens) override {
    int pos = tokens.getPos();
    if(left.parse(tokens)) {
      pos = tokens.getPos();
      Token opt;
      if(tokens(TOKEN_TYPE_EQUALS, &opt)) {
        op = opt.getValue();
        if(right.parse(tokens)) {
          hasRight = true;
          return true;
        } else {
          throw ParserError("Expected quaternary expression after operator " + op, tokens.getPos());
        }
      } else {
        hasRight = false;
        tokens.setPos(pos);
        return true;
      }
    }
    tokens.setPos(pos);
    return false;
  }

  virtual Object evaluate(Environment& e, Program& p) const override {
    if(hasRight) {
      Object o1 = left.evaluate(e, p);
      Object o2 = right.evaluate(e, p);
      return e.applyOperator(op, o1, o2); //should handle == just fine, and return classical |0> or |1>
    } else {
      return left.evaluate(e, p);
    }
  }
};

//may be function call or gate application
class ExpressionTypeFunctionCallAST : public ExpressionType {
  string name;
  vector<ExpressionTypeQuinaryAST*> expressions;
  vector<int> qubits;
public:
  virtual bool parse(TokenStream& tokens) override {
    //of form: identifier(expr, expr, ...)[list]
    int pos = tokens.getPos();

    Token tmp;
    if(tokens(TOKEN_TYPE_IDENTIFIER, &tmp) && parseListBounded(tokens, TOKEN_TYPE_COMMA, expressions)) {
      name = tmp.getValue();
      //additional list of integers at the end
      if(tokens(TOKEN_TYPE_BRACKET_LEFT)) {
        vector<string> qubitsTmp;
        parseSeparated(tokens, TOKEN_TYPE_LITERAL, TOKEN_TYPE_COMMA, qubitsTmp);
        if(TOKEN_TYPE_BRACKET_RIGHT) {
          for(const string& s : qubitsTmp) qubits.push_back(std::stoi(s));
          return true;
        } else {
          throw ParserError("Invalid bracket sequence after callable", tokens.getPos());
        }
      }
      return true;
    }

    tokens.setPos(pos);
    return false;
  }

  virtual Object evaluate(Environment& e, Program& p) const override {
    Function* f = p.getFunction(name);
    if(f) {
      int len = expressions.size();
      if(f->getArgs().size() == len) {
        //evaluate expressions
        vector<Object> objects(len);
        for(int i = 0; i < len; i++) {
          objects[i] = expressions[i]->evaluate(e, p);
        }
        int scopeLevel = e.push();
        //reassign values into new scope
        for(int i = 0; i < len; i++) {
          e[f->getArgs()[i]] = objects[i];
        }
        Object o = f->execute(e, p, qubits);
        if(e.pop() == scopeLevel) {
          return o;
        } else {
          throw ProgramError("Scope level mismatch detected");
        }
      } else {
        throw ProgramError("Argument count mismatch in function call");
      }
    } else {
      Object gate = e[name];
      if(gate) {
        if(gate.getType() == DATATYPE_GATE) {
          //apply gate to args
          if(expressions.size() == 1) {
            Object o = expressions[0]->evaluate(e, p);
            if(o.getType() == DATATYPE_STATE) {
              //yay!
              //actual application (with substate), return value
              if(qubits.size()) return e.applyGate(gate, o, qubits);
              else return e.applyGate(gate, o);
            } else {
              throw ProgramError("Attempted to apply gate to incorrect type");
            }
          } else {
            throw ProgramError("Attempted to apply gate to multiple variables");
          }
        } else {
          throw ProgramError("Attempted to call non-callable: " + name);
        }
      } else {
          throw ProgramError("Callable not found: " + name);
      }
    }
  }
};

class ExpressionAST {
  ExpressionType* expression;
public:
  bool parse(TokenStream& tokens) {
    int pos = tokens.getPos();

    //order: function call OR equality (...parens, ...)
    expression = new ExpressionTypeFunctionCallAST();
    if(expression->parse(tokens)) {
      return true;
    } else {
      delete expression;
      tokens.setPos(pos);
      expression = new ExpressionTypeQuinaryAST();
      if(expression->parse(tokens)) {
        return true;
      } else {
        delete expression;
        tokens.setPos(pos);
        return false;
      }
    }
  }

  Object evaluate(Environment& e, Program& p) const {
    if(expression) {
      return expression->evaluate(e, p);
    }
  }
};

//supertype for statements
class StatementType {
public:
  virtual bool parse(TokenStream& tokens) = 0;
  virtual Object execute(Environment& e, Program& p) const = 0;
  virtual Object evaluate(Environment& e, Program& p) const = 0;
};

//assigns a variable to the value of some expression
//assignment is optional, expression is not
class StatementTypeAssignAST : public StatementType {
  string assignTo; //may be empty
  ExpressionAST expr;
public:
  virtual bool parse(TokenStream& tokens) override {
    int pos = tokens.getPos();

    //assign part
    Token tmp;
    if(tokens(TOKEN_TYPE_IDENTIFIER, &tmp) && tokens(TOKEN_TYPE_ASSIGN)) {
      assignTo = tmp.getValue();
    } else {
      tokens.setPos(pos);
    }
    //value part
    if(expr.parse(tokens)) {
      return true;
    } else {
      tokens.setPos(pos);
      return false;
    }
  }

  virtual Object execute(Environment& e, Program& p) const override {
    //evaluate the expression
    Object o = expr.evaluate(e, p);
    if(assignTo.size()) e[assignTo] = o;
    //only return statment returns a value here, signifying function is complete
    return OBJECT_NONE;
  }

  virtual Object evaluate(Environment& e, Program& p) const override {
    //evaluate the expression
    Object o = expr.evaluate(e, p);
    if(assignTo.size()) e[assignTo] = o;
    return o;
  }
};

class StatementTypeReturnAST : public StatementType {
  ExpressionAST expr;
public:
  virtual bool parse(TokenStream& tokens) override {
    int pos = tokens.getPos();

    if(tokens(TOKEN_TYPE_KEYWORD_RETURN) && expr.parse(tokens)) {
      return true;
    }

    tokens.setPos(pos);
    return false;
  }

  virtual Object execute(Environment& e, Program& p) const override {
    return expr.evaluate(e, p);
  }

  virtual Object evaluate(Environment& e, Program& p) const override {
    return expr.evaluate(e, p);
  }
};

//other

class StatementAST {
  StatementType* statement;
public:
  bool parse(TokenStream& tokens) {
    int pos = tokens.getPos();

    statement = new StatementTypeReturnAST();
    if(statement->parse(tokens)) {
      return true;
    } else {
      delete statement;
      tokens.setPos(pos);
      statement = new StatementTypeAssignAST();
      if(statement->parse(tokens)) {
        return true;
      } else {
        delete statement;
        tokens.setPos(pos);
        return false;
      }
    }
  }

  Object execute(Environment& e, Program& p) const {
    if(statement) {
      return statement->execute(e, p);
    }
    return OBJECT_NONE;
  }

  Object evaluate(Environment& e, Program& p) const {
    if(statement) {
      return statement->evaluate(e, p);
    }
    return OBJECT_NONE;
  }
};

class FunctionPrototypeAST {
  string name;
  vector<string> args;
public:
  const string getName() const {
    return name;
  }

  const vector<string>& getArgs() const {
    return args;
  }

  bool parse(TokenStream& tokens) {
    int pos = tokens.getPos();

    Token id;
    if(tokens(TOKEN_TYPE_KEYWORD_FUNCTION) && tokens(TOKEN_TYPE_IDENTIFIER, &id)) {
      name = id.getValue();
      //parse args
      if(tokens(TOKEN_TYPE_PARENS_LEFT)) {
        parseSeparated(tokens, TOKEN_TYPE_IDENTIFIER, TOKEN_TYPE_COMMA, args);
        if(tokens(TOKEN_TYPE_PARENS_RIGHT)) return true;
      }
    }

    tokens.setPos(pos);
    return false;
  }
};

//gotta put this here to get rid of compiler errors...
Function::~Function() {
}

class FunctionAST : public Function {
  FunctionPrototypeAST proto;
  vector<StatementAST*> statements;
public:
  const string getName() const override {
    return proto.getName();
  }

  const vector<string>& getArgs() const override {
    return proto.getArgs();
  }

  bool parse(TokenStream& tokens) {
    int pos = tokens.getPos();

    if(proto.parse(tokens)) {
      if(tokens(TOKEN_TYPE_BRACE_LEFT)) {
        parseSome<StatementAST>(tokens, statements);
        if(tokens(TOKEN_TYPE_BRACE_RIGHT)) {
          return true;
        }
      }
    }
    tokens.setPos(pos);
    return false;
  }

  virtual Object execute(Environment& e, Program& p, const vector<int>& substate) const override {
    Object tmp;
    for(StatementAST* s : statements) {
      //only "return" statement should return a value
      if((tmp = s->execute(e, p))) return tmp;
    }
    return OBJECT_NONE;
  }
};

class ProgramAST : public Program {
public:
  void parse(TokenStream& tokens) {
    vector<FunctionAST*> tmp;
    parseSome<FunctionAST>(tokens, tmp);
    for(Function* f : tmp) addFunction(f);
  }
};

//builtin functions

class FunctionBuiltin : public Function {
string name;
vector<string> args;
public:
  FunctionBuiltin(string name_, std::initializer_list<string> args_) : name(name_), args(args_) {}

  const string getName() const override { return name; }
  const vector<string>& getArgs() const override { return args; }
};

//TODO: built-in function subclasses
//control
//R_k
//swap
//measure
//dim

class FunctionBuiltinControl : public FunctionBuiltin {
public:
  FunctionBuiltinControl() : FunctionBuiltin("control", {"U"}) {}
  Object execute(Environment& e, Program& p, const vector<int>& substate) const override {
    if(substate.size()) {
      //place control in a specific place (TODO)
      return OBJECT_NONE;
    }
    return e.createObject(QuantumGate::control(e["U"].castData<QuantumGate>()));
  }
};

class FunctionBuiltinRk : public FunctionBuiltin {
public:
  FunctionBuiltinRk() : FunctionBuiltin("R", {"k"}) {}
  Object execute(Environment& e, Program& p, const vector<int>& substate) const override {
    return e.createObject(QuantumGate::R(e["k"].castData<int>()));
  }
};

class FunctionBuiltinSwap : public FunctionBuiltin {
public:
  FunctionBuiltinSwap() : FunctionBuiltin("swap", {}) {}
  Object execute(Environment& e, Program& p, const vector<int>& substate) const override {
    return e.createObject(QuantumGate::swap(substate));
  }
};

class FunctionBuiltinMeasure : public FunctionBuiltin {
public:
  FunctionBuiltinMeasure() : FunctionBuiltin("measure", {"x"}) {}
  Object execute(Environment& e, Program& p, const vector<int>& substate) const override {
    QuantumState& q = e.getState();
    vector<int>& qubits = e["x"].castData<vector<int>>();
    vector<int> measured;
    if(substate.size()) for(int qubit : substate) measured.push_back(qubits[qubit]);
    else measured = qubits;
    int choice;
    q.measure_(measured, &choice);
    return e.createObject(choice);
  }
};

class FunctionBuiltinDim : public FunctionBuiltin {
public:
  FunctionBuiltinDim() : FunctionBuiltin("dim", {"x"}) {}
  Object execute(Environment& e, Program& p, const vector<int>& substate) const override {
    Object o = e["x"];
    switch(o.getType()) {
      case Qwak::DATATYPE_STATE:
        return e.createObject(o.castData<vector<int>>().size());
      case Qwak::DATATYPE_GATE:
        return e.createObject(o.castData<Qwality::QuantumGate>().getN());
      case Qwak::DATATYPE_INT:
        return e.createObject(1);
      default:
        return OBJECT_NONE;
    }
  }
};

//public API stuff

void Program::addFunction(Function* f) {
  if(functionsByName[f->getName()] == nullptr) functions.push_back(f); //it's a new function
  functionsByName[f->getName()] = f;
}

bool Program::tryAddFunction(Function* f) {
  //no overriding previous functions
  if(functionsByName[f->getName()] != nullptr) return false;

  functions.push_back(f);
  functionsByName[f->getName()] = f;
  return true;
}

Function* Program::getFunction(string name) const {
  return (functionsByName.find(name) != functionsByName.end()) ? functionsByName.at(name) : nullptr;
}

const vector<Function*>& Program::getFunctions() const {
  return functions;
}

void Program::addBuiltinFunctions() {
  addFunction(new FunctionBuiltinControl());
  addFunction(new FunctionBuiltinRk());
  addFunction(new FunctionBuiltinSwap());
  addFunction(new FunctionBuiltinMeasure());
  addFunction(new FunctionBuiltinDim());
}

QwakParser::QwakParser() : version_("v1.0.0") {
  //TODO: add a bunch of token rules

  tokenRules.push_back(new TokenRuleWhitespace());

  //symbols

  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_PARENS_LEFT, "("));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_PARENS_RIGHT, ")"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_BRACKET_LEFT, "["));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_BRACKET_RIGHT, "]"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_BRACE_LEFT, "{"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_BRACE_RIGHT, "}"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_ANGLE_LEFT, "<"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_ANGLE_RIGHT, ">"));

  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_COMMA, ","));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_DOTS, ".."));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_PIPE, "|"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_EQUALS, "=="));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_ASSIGN, "="));

  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_PLUS, "+"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_MINUS, "-"));
  //rules must be in longest-prefix order...
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_TENSOR_EXPONENT, "^**"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_EXPONENT, "**"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_TENSOR, "^*"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_CONCAT, "^"));

  //keywords

  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_KEYWORD_FUNCTION, "function"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_KEYWORD_RETURN, "return"));

  //other...

  tokenRules.push_back(new TokenRuleNumeric(true, TOKEN_TYPE_LITERAL));
  tokenRules.push_back(new TokenRuleAlphabetic(true, TOKEN_TYPE_IDENTIFIER));

  //message of the day :^)
  //https://stackoverflow.com/a/25021520
  const char *s =
#include "name.txt"
  ;
  std::string s2(s);
  std::size_t pos = 0;
  std::size_t pos2 = 0;
  do {
    pos2 = s2.find('\n', pos);
    std::string sub = s2.substr(pos, pos2 - pos);
    if(sub[0] == 'Q' || sub[0] == 'q') messages.push_back(sub);
    pos = pos2 + 1;
  } while(pos2 < std::string::npos);
  srand((unsigned)time(0));
}

const std::string& QwakParser::version() {
  return version_;
}

//message of the day
const std::string& QwakParser::motd() {
  //generate random pos
  int pos = rand() % messages.size();
  return messages[pos];
}

Program* QwakParser::createEmptyProgram() {
  return new ProgramAST();
}

Program* QwakParser::parse(const std::string& buffer) {
  TokenStream stream(buffer);
  stream.addRules(tokenRules);

  ProgramAST* p = new ProgramAST();
  p->parse(stream);
  return p;
}

Object QwakParser::execute(const std::string& statement, Program& p, Environment& e) {
  TokenStream stream(statement);
  stream.addRules(tokenRules);

  StatementAST s;
  if(s.parse(stream)) {
    return s.evaluate(e, p);
  }
  return OBJECT_NONE;
}

ProgramError::ProgramError(const string& what) : QwakError(what) {}

} //end namespace