#include "parse.h"

namespace Qwak {

Token::Token(int type_, string value_) : type(type_), value(value_) {}

int Token::getType() {
  return type;
}

const string& Token::getValue() {
  return value;
}

TokenRule::TokenRule(bool keepToken__, int tokenType_) : keepToken_(keepToken__), tokenType(tokenType_) {}

bool TokenRule::keepToken() {
  return keepToken_;
}

TokenRuleClass::TokenRuleClass(bool keepToken__, int tokenType_, const string& chars_) : TokenRule(keepToken__, tokenType_), chars(chars_) {}

int TokenRuleClass::apply(const string& buffer, int begin, Token* fill) {
  int end = begin;
  while(chars.find(buffer[end]) != string::npos) end++;
  *fill = Token(tokenType, buffer.substr(begin, end - begin));
  return end;
}

TokenRuleAlphabetic::TokenRuleAlphabetic(bool keepToken__, int tokenType_) : TokenRuleClass(keepToken__, tokenType_, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") {}

TokenRuleNumeric::TokenRuleNumeric(bool keepToken__, int tokenType_) : TokenRuleClass(keepToken__, tokenType_, "0123456789") {}

TokenRuleExact::TokenRuleExact(bool keepToken__, int tokenType_, string match_) : TokenRule(keepToken__, tokenType_), match(match_) {}

int TokenRuleExact::apply(const string& buffer, int begin, Token* fill) {
  std::size_t len = match.size();
  if(buffer.size() - begin < len) return begin;
  if(buffer.substr(begin, len) == match) return begin + len;
  return begin;
}

TokenRuleWhitespace::TokenRuleWhitespace() : TokenRule(false, TOKEN_TYPE_WHITESPACE) {}

int TokenRuleWhitespace::apply(const string& buffer, int begin, Token* fill) {
  char c = buffer[begin];
  if(c == ' ' || c == '\t' || c == '\n' || c == '\r') {
    *fill = Token(tokenType, std::string(1, c));
    return begin + 1;
  }
  return begin;
}

TokenStream::TokenStream(string buffer_) : buffer(buffer_), curPos(0), hasTokenized(false) {
}

void TokenStream::addRule(TokenRule* rule) {
  rules.push_back(rule);
  hasTokenized = false;
}

void TokenStream::addRules(const vector<TokenRule*> rules) {
  for(TokenRule* rule : rules) this->rules.push_back(rule);
  hasTokenized = false;
}

void TokenStream::tokenize() {
  tokens.clear();

  //tokenize here
  int pos = 0;
  Token tmp;
  while(pos < buffer.size()) {
    int newPos = pos;
    for(auto rule : rules) {
      newPos = rule->apply(buffer, pos, &tmp);
      if(newPos != pos) {
        tokens.push_back(tmp);
        break;
      }
    }
    if(newPos == pos) {
      //was unable to match any token
      throw TokenizerError("Unable to tokenize input", pos);
    }
  }

  hasTokenized = true;
  curPos = 0;
}

TokenStream::operator bool() {
  if(!hasTokenized) {
    tokenize();
  }
  return curPos < tokens.size();
}

Token TokenStream::operator*() {
  return tokens[curPos];
}

TokenStream& TokenStream::operator++() { //prefix
  curPos++;
  return *this;
}

void TokenStream::operator++(int) { //postfix
  curPos++;
}

bool TokenStream::operator()(int tokenType, Token* t) {
  Token& tmp = tokens[curPos];
  if(tmp.getType() == tokenType) {
    if(t) *t = tmp;
    curPos++;
    return true;
  }
  return false;
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
    parent.addChild(TokenTree(t));
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
    parent.addChild(TokenTree(t));
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
  TokenTree child;
  for(auto rule : rules) {
    if(!rule->apply(stream, child)) {
      stream.setPos(pos);
      return false;
    }
  }
  parent.addChild(child);
  return true;
}

GrammarRuleOr::GrammarRuleOr(initializer_list<GrammarRule*> rules_) : GrammarRuleCompound(rules_) {}

bool GrammarRuleOr::apply(TokenStream& stream, TokenTree& parent) {
  int pos = stream.getPos();
  for(auto rule : rules) {
    stream.setPos(pos);
    //if false, the rule must implicitly reset the stream position
    if(rule->apply(stream, parent)) {
      return true;
    }
  }
  return false;
}

//0 or more
GrammarRuleMulti* GrammarRuleMulti::some(GrammarRule* base) {
  return new GrammarRuleMulti(base, 0, -1);
}

//1 or more
GrammarRuleMulti* GrammarRuleMulti::many(GrammarRule* base) {
  return new GrammarRuleMulti(base, 1, -1);
}

//0 or 1
GrammarRuleMulti* GrammarRuleMulti::maybe(GrammarRule* base) {
  return new GrammarRuleMulti(base, 0, 1);
}

GrammarRuleMulti::GrammarRuleMulti(GrammarRule* base_, int min_, int max_) : base(base_), min(min_), max(max_) {}

bool GrammarRuleMulti::apply(TokenStream& stream, TokenTree& parent) {
  TokenTree child;
  int count = 0;
  while(max < 0 || count < max) {
    if(base->apply(stream, child)) {
      count++;
    } else {
      break;
    }
  }
  if(count >= min) {
    parent.addChild(child);
    return true;
  } else {
    return false;
  }
}

//error types

QwakError::QwakError(const string& what) : std::runtime_error(what) {}

TokenizerError::TokenizerError(const string& what, int pos_) : QwakError(what), pos(pos_) {}

int TokenizerError::getPos() {
  return pos;
}

ParserError::ParserError(const string& what, int token_) : QwakError(what), token(token_) {}

int ParserError::getToken() {
  return token;
}

} //end namespace

using namespace Qwak;

GrammarRule* operator&(GrammarRule& left, GrammarRule& right) {
  return new GrammarRuleAnd{&left, &right};
}

GrammarRule* operator|(GrammarRule& left, GrammarRule& right) {
  return new GrammarRuleOr{&left, &right};
}