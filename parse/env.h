#ifndef PARSE_H
#define PARSE_H

#include <string>

namespace Qwak {

//represents an object in a quantum environment
//has type and data
//TODO: could have implementor subtypes for each programmable type
//so that the implementors could virtual-override all necessary interfaces
class Object {

};

//encapsulates the quantum state and other variables within a function
class Environment {
private:

public:
  //check existence of variable
  bool operator()(const std::string& variable);
  //create or modify variable
  //this won't let us override Object, but we can set a variable inside it
  Object& operator[](const std::string& variable);
};

//TODO: API for parser
//void parse(string)?
//Function parse(string)?
//Object execute(string)?
class Parser {
private:

public:
  //?
};

} //end namespace

#endif //PARSE_H