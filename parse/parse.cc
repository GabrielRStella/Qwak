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

void TokenStream::addRule(TokenRule* rule) {
}

TokenStream::operator bool() {
}

Token TokenStream::operator*() {
}

TokenStream& TokenStream::operator++() {
}

int TokenStream::getPos() {
  return curPos;
}

void TokenStream::setPos(int pos) {
  curPos = pos;
}

TokenTree::TokenTree(Token current_) : current(current_) {}

vector<TokenTree>& TokenTree::getChildren() {
}

TokenTree& TokenTree::addChild(TokenTree t) {
}

GrammarRuleTokenType::GrammarRuleTokenType(int tokenType_) : tokenType(tokenType_) {}

bool GrammarRuleTokenType::apply(TokenStream& stream, TokenTree& parent) {
}

GrammarRuleTokenValue::GrammarRuleTokenValue(string tokenValue_) : tokenValue(tokenValue_) {}

bool GrammarRuleTokenValue::apply(TokenStream& stream, TokenTree& parent) {
}

GrammarRuleCompound::GrammarRuleCompound(initializer_list<GrammarRule*> rules_) : rules(rules_) {}

GrammarRuleAnd::GrammarRuleAnd(initializer_list<GrammarRule*> rules_) : GrammarRuleCompound(rules_) {}

bool GrammarRuleAnd::apply(TokenStream& stream, TokenTree& parent) {
}

GrammarRuleOr::GrammarRuleOr(initializer_list<GrammarRule*> rules_) : GrammarRuleCompound(rules_) {}

bool GrammarRuleAnd::apply(TokenStream& stream, TokenTree& parent) {
}

} //end namespace

Qwak::GrammarRule* operator&(Qwak::GrammarRule* left, Qwak::GrammarRule* right) {
}

Qwak::GrammarRule* operator|(Qwak::GrammarRule* left, Qwak::GrammarRule* right) {
}