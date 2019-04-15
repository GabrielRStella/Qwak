#ifndef QWAK_H
#define QWAK_H

#include "parse.h"

namespace Qwak {

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