#include "qwak.h"

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
//ops
const int TOKEN_TYPE_PLUS = 120;
const int TOKEN_TYPE_MINUS = 121;
const int TOKEN_TYPE_CONCAT = 122; // ^ for |0^n> etc
const int TOKEN_TYPE_EXPONENT = 122; // **
const int TOKEN_TYPE_TENSOR = 122; // ^*
const int TOKEN_TYPE_TENSOR_EXPONENT = 122; // ^**
//keywords
const int TOKEN_TYPE_KEYWORD_FUNCTION = 200;
const int TOKEN_TYPE_KEYWORD_RETURN = 201;
//other
const int TOKEN_TYPE_LITERAL = 300; //literal of some sort: state, number, list, etc
const int TOKEN_TYPE_IDENTIFIER = 301; //identifier, e.g. a variable or function name

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

//semantic elements

//supertype for expressions
class ExpressionType {
public:
  virtual bool parse(TokenStream& tokens) = 0;
  virtual Object execute(Environment& e, Program& p) const = 0;
};

//TODO: more expression types
/*
class ExpressionType : public ExpressionType {
public:
  virtual bool parse(TokenStream& tokens) override {
    //TODO
    return false;
  }

  virtual Object execute(Environment& e, Program& p) const override {
    
  }
};
*/

//may be function call or gate application
class ExpressionTypeFunctionCallAST : public ExpressionType {
  string name;
  vector<ExpressionType*> expressions;
public:
  virtual bool parse(TokenStream& tokens) override {
    //TODO
    //of form: identifier(expr, expr, ...)[list]

    return false;
  }

  virtual Object execute(Environment& e, Program& p) const override {
    Function* f = p.getFunction(name);
    if(f) {
      int len = expressions.size();
      if(f->getArgs().size() == len) {
        //evaluate expressions
        vector<Object> objects(len);
        for(int i = 0; i < len; i++) {
          objects[i] = expressions[i]->execute(e, p);
        }
        int scopeLevel = e.push();
        //reassign values into new scope
        for(int i = 0; i < len; i++) {
          e[f->getArgs()[i]] = objects[i];
        }
        Object o = f->execute(e, p);
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
            Object o = expressions[0]->execute(e, p);
            if(o.getType() == DATATYPE_STATE) {
              //yay!

              //TODO: actual application, return value
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
    //TODO

    return false;
  }

  Object execute(Environment& e, Program& p) const {
    if(expression) {
      return expression->execute(e, p);
    }
  }
};

//supertype for statements
class StatementType {
public:
  virtual bool parse(TokenStream& tokens) = 0;
  virtual Object execute(Environment& e, Program& p) const = 0;
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
    Object o = expr.execute(e, p);
    if(assignTo.size()) e[assignTo] = o;
    //only return statment returns a value here, signifying function is complete
    return OBJECT_NONE;
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
    return expr.execute(e, p);
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

  virtual Object execute(Environment& e, Program& p) const override {
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

//public API stuff

void Program::addFunction(Function* f) {
  //no overriding previous functions
  if(functionsByName.at(f->getName()) != nullptr) throw QwakError("Attempted to override function definition: " + f->getName());

  functions.push_back(f);
  functionsByName[f->getName()] = f;
}

Function* Program::getFunction(string name) const {
  return functionsByName.at(name);
}

const vector<Function*>& Program::getFunctions() const {
  return functions;
}

QwakParser::QwakParser() {
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
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_ASSIGN, "="));

  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_PLUS, "+"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_MINUS, "-"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_CONCAT, "^"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_EXPONENT, "**"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_TENSOR, "^*"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_TENSOR_EXPONENT, "^**"));

  //keywords

  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_KEYWORD_FUNCTION, "function"));
  tokenRules.push_back(new TokenRuleExact(true, TOKEN_TYPE_KEYWORD_RETURN, "return"));

  //other...
}

Program* QwakParser::parse(const std::string& buffer) {
  TokenStream stream(buffer);
  for(TokenRule* rule : tokenRules) stream.addRule(rule);

  ProgramAST* p = new ProgramAST();
  p->parse(stream);
  return p;
}

ProgramError::ProgramError(const string& what) : QwakError(what) {}

} //end namespace