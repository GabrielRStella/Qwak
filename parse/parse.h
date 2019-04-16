#ifndef PARSE_H
#define PARSE_H

#include <stack>
#include <string>
#include <vector>
#include <initializer_list>
#include <regex>
#include <stdexcept>

//note: none of the stuff in this file is specific to Qwak.
//it is just a general language definition/parsing toolkit.

/*
TODO:
-error handling (be able to return failure points to user)
*/

namespace Qwak {

using std::stack;
using std::string;
using std::vector;
using std::initializer_list;
using std::regex;

//token types:
//ones for logistical reasons
const int TOKEN_TYPE_NON = 0; //placeholder for invalid tokens
const int TOKEN_TYPE_ROOT = 1; //placeholder for a "root" token
const int TOKEN_TYPE_WHITESPACE = 2;
//some simple predefined ones (start at 10 to allow more above) (just ideas)
//const int TOKEN_TYPE_KEYWORD = 10; //a token that is a keyword
//const int TOKEN_TYPE_OPERATOR = 11; //a token for some sort of operator, e.g. +-*/^
//const int TOKEN_TYPE_LITERAL = 12; //a token that is a literal of some sort (e.g. a number)
//const int TOKEN_TYPE_IDENTIFIER = 13; //identifier, e.g. a variable or function

class Token {
private:
  int type; //the type of token for consistency: eg. TYPE_LEFT_PARENS, TYPE_VAR_DECL, etc
  string value; //the actual string

public:
  Token(int type = TOKEN_TYPE_NON, string value = "");

  int getType();
  const string& getValue();
};

const Token EMPTY_TOKEN;

//rule used to parse tokens
class TokenRule {
private:
  bool keepToken_; //return this value in keepToken

protected:
  int tokenType;

public:
  TokenRule(bool keepToken__, int tokenType);

  //if return != begin, the rule is considered to have worked
  //return = the point to begin parsing the next token
  virtual int apply(const string& buffer, int begin, Token* fill) = 0;

  //if the token should be kept; for example,
  //a token rule could be defined that accepts all whitespace
  //and returns false here.
  bool keepToken();
};

class TokenRuleRegex : public TokenRule {
private:
  regex pattern;

public:
  TokenRuleRegex(bool keepToken__, int tokenType, regex pattern);

  virtual int apply(const string& buffer, int begin, Token* fill) override;
};

class TokenRuleExact : public TokenRule {
private:
  string match;

public:
  TokenRuleExact(bool keepToken__, int tokenType, string match);

  virtual int apply(const string& buffer, int begin, Token* fill) override;
};

class TokenRuleWhitespace : public TokenRule {
public:
  TokenRuleWhitespace();

  virtual int apply(const string& buffer, int begin, Token* fill) override;
};

//extracts tokens from a buffer (i.e. a string)
class TokenStream {
private:
  string buffer;

  int curPos;

  vector<TokenRule*> rules;

  bool hasTokenized;
  vector<Token> tokens;

  void tokenize();

public:
  TokenStream(string buffer);

  //adds another token rule. the new rule will be lower priority than the previous rules.
  void addRule(TokenRule* tokenRule);

  //check if there is another token to extract
  //i.e., if the next *dereference will succeed
  operator bool();

  //access tokens, move stream forward
  Token operator*();
  TokenStream& operator++(); //prefix
  void operator++(int); //postfix

  //checks for a single token of the given type
  //if successful, returns token in pointer (unless t == nullptr)
  //and also moves forward
  bool operator()(int tokenType, Token* t = nullptr);

  //used for saving the position of the stream (in case a GrammarRule fails)
  int getPos();
  void setPos(int pos);
};

class TokenTree {
private:
  Token current;
  vector<TokenTree> children;

public:
  TokenTree(Token current = EMPTY_TOKEN);

  vector<TokenTree>& getChildren();

  //add child, return a reference to the stored copy
  TokenTree& addChild(const TokenTree& t);
};

//because GrammarRule is overridable, they must always be dealt with as pointers or references
//otherwise virtual functions will not be applied correctly
//(same for TokenRule)
class GrammarRule {
private:

public:
  //attempt to apply this grammar rule to the tokens in the given stream
  //if successful, it will add a child to the given parent tree.
  //returns true if successful.
  virtual bool apply(TokenStream& stream, TokenTree& parent) = 0;
};

class GrammarRuleTokenType : public GrammarRule {
private:
  int tokenType;

public:
  GrammarRuleTokenType(int tokenType);

  virtual bool apply(TokenStream& stream, TokenTree& parent) override;
};

class GrammarRuleTokenValue : public GrammarRule {
private:
  string tokenValue;

public:
  GrammarRuleTokenValue(string tokenValue);

  virtual bool apply(TokenStream& stream, TokenTree& parent) override;
};

//supertype for compound rules, composed of several other rules
//TODO: if any of the sub-rules in this are the same type, can maybe compress the tree
class GrammarRuleCompound : public GrammarRule {
protected:
  vector<GrammarRule*> rules;

public:
  GrammarRuleCompound(initializer_list<GrammarRule*> rules);
};

//"and" rules, e.g. "<ident> AND '+' AND <ident>"
//will insert a fake node between parent and children... seems unavoidable, unfortunately
class GrammarRuleAnd : public GrammarRuleCompound {
public:
  GrammarRuleAnd(initializer_list<GrammarRule*> rules);

  virtual bool apply(TokenStream& stream, TokenTree& parent) override;
};

//"or" rules, e.g. "<ident> OR <literal>"
class GrammarRuleOr : public GrammarRuleCompound {
public:
  GrammarRuleOr(initializer_list<GrammarRule*> rules);

  virtual bool apply(TokenStream& stream, TokenTree& parent) override;
};

//multi grammar rules, e.g. "<ident>*", "<ident>?", or "<ident>+"
class GrammarRuleMulti : public GrammarRule {
public:
  //0 or more
  static GrammarRuleMulti* some(GrammarRule* base);
  //1 or more
  static GrammarRuleMulti* many(GrammarRule* base);
  //0 or 1
  static GrammarRuleMulti* maybe(GrammarRule* base);

private:
  GrammarRule* base;
  int min;
  int max;

public:
  GrammarRuleMulti(GrammarRule* base, int min, int max);

  virtual bool apply(TokenStream& stream, TokenTree& parent) override;
};

//error types

class QwakError : public std::runtime_error {
public:
  QwakError(const string& what);
};

class TokenizerError : public QwakError {
  int pos;
public:
  TokenizerError(const string& what, int pos);

  //the offset where the error occurred (right after the last successfully-parsed token)
  int getPos();
};

class ParserError : public QwakError {
  int token;
public:
  ParserError(const string& what, int token);

  int getToken();
};

} //end namespace

//operators for composing grammar rules (and, or)
//TODO: operator for GrammarRuleMulti?

Qwak::GrammarRule* operator&(Qwak::GrammarRule& left, Qwak::GrammarRule& right);

Qwak::GrammarRule* operator|(Qwak::GrammarRule& left, Qwak::GrammarRule& right);

#endif //PARSE_H