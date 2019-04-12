#include "parse.h"

namespace Qwak {

Token::Token(int type_, string value_) : type(type_), value(value_) {}

int Token::getType() {
  return type;
}

const string& Token::getValue() {
  return value;
}

TokenRule::TokenRule(bool keepToken__) : keepToken_(keepToken__) {}

bool TokenRule::keepToken() {
  return keepToken_;
}

TokenStream::TokenStream(string buffer_) : buffer(buffer_), curPos(0) {
}

//TODO GS
void TokenStream::addRule(TokenRule* rule) {
}

//TODO GS
TokenStream::operator bool() {
}

//TODO GS
Token TokenStream::operator*() {
}

//TODO GS
TokenStream& TokenStream::operator++() { //prefix
}

//TODO GS
TokenStream& TokenStream::operator++(int) { //postfix
}

int TokenStream::getPos() {
  return curPos;
}

void TokenStream::setPos(int pos) {
  curPos = pos;
}

TokenTree::TokenTree(Token current_) : current(current_) {}

vector<TokenTree>& TokenTree::getChildren() {
  return children;
}

TokenTree& TokenTree::addChild(const TokenTree& t) {
  children.push_back(t);
  return children[children.size() - 1];
}

GrammarRuleTokenType::GrammarRuleTokenType(int tokenType_) : tokenType(tokenType_) {}

bool GrammarRuleTokenType::apply(TokenStream& stream, TokenTree& parent) {
  if(!stream) return false;
  Token t = *stream;
  if(t.getType() == tokenType) {
    stream++;
    return true;
  } else {
    return false;
  }
}

GrammarRuleTokenValue::GrammarRuleTokenValue(string tokenValue_) : tokenValue(tokenValue_) {}

bool GrammarRuleTokenValue::apply(TokenStream& stream, TokenTree& parent) {
  if(!stream) return false;
  Token t = *stream;
  if(t.getValue() == tokenValue) {
    stream++;
    return true;
  } else {
    return false;
  }
}

GrammarRuleCompound::GrammarRuleCompound(initializer_list<GrammarRule*> rules_) : rules(rules_) {}

GrammarRuleAnd::GrammarRuleAnd(initializer_list<GrammarRule*> rules_) : GrammarRuleCompound(rules_) {}

bool GrammarRuleAnd::apply(TokenStream& stream, TokenTree& parent) {
  int pos = stream.getPos();
  for(auto rule : rules) {
    if(!rule->apply(stream, parent)) {
      stream.setPos(pos);
      return false;
    }
  }
  return true;
}

GrammarRuleOr::GrammarRuleOr(initializer_list<GrammarRule*> rules_) : GrammarRuleCompound(rules_) {}

bool GrammarRuleOr::apply(TokenStream& stream, TokenTree& parent) {
  int pos = stream.getPos();
  for(auto rule : rules) {
    stream.setPos(pos);
    //if false, the rule must implicitly reset the stream position
    if(rule->apply(stream, parent)) return true;
  }
  return false;
}

} //end namespace

using namespace Qwak;

GrammarRule* operator&(GrammarRule& left, GrammarRule& right) {
  return new GrammarRuleAnd{&left, &right};
}

GrammarRule* operator|(GrammarRule& left, GrammarRule& right) {
  return new GrammarRuleOr{&left, &right};
}