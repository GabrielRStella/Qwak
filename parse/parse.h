#ifndef PARSE_H
#define PARSE_H

#include <stack>
#include <string>
#include <vector>
#include <initializer_list>

//note: none of the stuff in this file is specific to Qwak.
//it is just a general language definition/parsing toolkit.

namespace Qwak {

using std::stack;
using std::string;
using std::vector;
using std::initializer_list;

//token types:
//ones for logistical reasons
const int TOKEN_TYPE_NON = 0; //placeholder for invalid tokens
const int TOKEN_TYPE_ROOT = 1; //placeholder for a "root" token
//some simple predefined ones
const int TOKEN_TYPE_KEYWORD = 2; //a token that is a keyword
const int TOKEN_TYPE_OPERATOR = 3; //a token for some sort of operator, e.g. +-*/^
const int TOKEN_TYPE_LITERAL = 4; //a token that is a literal of some sort (e.g. a number)
const int TOKEN_TYPE_IDENTIFIER = 5; //identifier, e.g. a variable or function
//TODO: user (us...) should define other token types (TOKEN_TYPE_OP_PLUS, etc)

class Token {
private:
  int type; //the type of token for consistency: eg. TYPE_LEFT_PARENS, TYPE_VAR_DECL, etc
  string value; //the actual string

public:
  Token(int type = TOKEN_TYPE_NON, string value = "");

  int getType();
  const string& getValue();
};

//extracts tokens from a buffer (i.e. a string)
class TokenStream {
private:
  string buffer;

  stack positions;
  int curPos;

  //TODO: other stuff?

public:
  TokenStream(string buffer);

  //adds another token rule. the new rule will be lower priority than the previous rules.
  void addRule(TokenRule* tokenRule);

  //check if there is another token to extract
  operator bool();

  //access tokens, move stream forward
  Token operator*();
  TokenStream& operator++();

  //used for saving the position of the stream (in case a GrammarRule fails)
  int getPos();
  void setPos(int pos);
};

//rule used to parse tokens
class TokenRule {
private:
  bool keepToken_; //return this value in keepToken

public:
  TokenRule(bool keepToken);

  //if return != begin, the rule is considered to have worked
  //return = the point to begin parsing the next token
  virtual int apply(string buffer, int begin, Token* fill);

  //if the token should be kept; for example,
  //a token rule could be defined that accepts all whitespace
  //and returns false here.
  bool keepToken();
};

class TokenTree {
private:
  Token current;
  vector<TokenTree> children;

public:
  TokenTree(Token current);

  vector<TokenTree>& getChildren();

  //add child, return a reference to the stored copy
  TokenTree& addChild(TokenTree t);
};

//because GrammarRule is overridable, they must always be dealt with as pointers
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

} //end namespace

//operators for composing grammar rules (and, or)

GrammarRule* operator&(GrammarRule* left, GrammarRule* right);

GrammarRule* operator|(GrammarRule* left, GrammarRule* right);

#endif //PARSE_H