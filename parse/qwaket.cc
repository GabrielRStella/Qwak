#include "qwak.h"

#include <iostream>
#include <fstream>
#include <unordered_map>

using namespace Qwak;

using namespace std;

/*
TODO:
-commands
-QFT(m) builtin
-QFT(m) inverse builtin
*/

typedef int (*CMD)(QwakParser&, Program&, Environment&, const string&);

int cmd_motd(QwakParser& parser, Program& p, Environment& e, const string& args) {
  cout << "MOTD: " << parser.motd() << endl;
  return 0;
}

int cmd_load(QwakParser& parser, Program& p, Environment& e, const string& args_) {
  string args = args_;
  args = args + ".qwak";
  cout << "Loading \"" << args << "\"..." << endl;

  //https://stackoverflow.com/q/2602013
  std::ifstream t;
  t.open(args);      // open input file
  if(!t.good()) return -1;
  t.seekg(0, std::ios::end);    // go to the end
  auto length = t.tellg();           // report location (this is the length)
  t.seekg(0, std::ios::beg);    // go back to the beginning
  char* buffer = new char[length];    // allocate memory for a buffer of appropriate dimension
  t.read(buffer, length);       // read the whole file into the buffer
  t.close();                    // close file handle

  //copy all loaded functions
  Program* p2 = parser.parse(string(buffer));
  auto fs = p2->getFunctions();
  cout << "Loaded " << fs.size() << " functions:" << endl;
  for(Function* f : fs) {
    cout << " " << f->getName() << "(";
    auto args = f->getArgs();
    for(auto i = 0; i < args.size(); i++) {
      if(i) cout << ", ";
      cout << args[i];
    }
    cout << ")" << endl;
    p.addFunction(f);
  }

  return 0;
}

int cmd_state(QwakParser& parser, Program& p, Environment& e, const string& args) {
  cout << "State: " << e.getState() << endl;
  return 0;
}

int cmd_func(QwakParser& parser, Program& p, Environment& e, const string& args) {
  cout << "Functions:" << endl;
  for(Function* f : p.getFunctions()) {
    cout << " " << f->getName() << "(";
    auto args = f->getArgs();
    for(auto i = 0; i < args.size(); i++) {
      if(i) cout << ", ";
      cout << args[i];
    }
    cout << ")" << endl;
  }
  return 0;
}

int cmd_env(QwakParser& parser, Program& p, Environment& e, const string& args) {
  cout << "Variables:" << endl;
  vector<string> vars;
  e.getVariables(vars);
  for(string var : vars) cout << " " << var << endl;
  return 0;
}

int cmd_clear(QwakParser& parser, Program& p, Environment& e, const string& args) {
  e.reset();
  return 1;
}

int cmd_del(QwakParser& parser, Program& p, Environment& e, const string& args) {
  Object& o = e[args];
  if(o) {
    o = OBJECT_NONE;
    return 1;
  } else return -1;
}

int cmd_deutsch(QwakParser& parser, Program& p, Environment& e, const string& args) {
  QuantumGate ua = QuantumGate::makeUnitary([](bool x){return false;});
  QuantumGate ub = QuantumGate::makeUnitary([](bool x){return x;});
  QuantumGate uc = QuantumGate::makeUnitary([](bool x){return !x;});
  QuantumGate ud = QuantumGate::makeUnitary([](bool x){return true;});
  cout << "Creating Deutsch demo unitaries..." << endl;
  cout << " ua(x) = 0" << endl;
  cout << " ub(x) = x" << endl;
  cout << " uc(x) = !x" << endl;
  cout << " ud(x) = 1" << endl;
  e["ua"] = e.createObject(ua);
  e["ub"] = e.createObject(ub);
  e["uc"] = e.createObject(uc);
  e["ud"] = e.createObject(ud);
}

int main() {

  QwakParser parser;

  Program* pr = parser.createEmptyProgram();
  Program& p = *pr;

  p.addBuiltinFunctions();

  Environment e;

  cout << "Environment created! Running QWAK " << parser.version() << ": \"" << parser.motd() << "\"" << endl;

  unordered_map<std::string, CMD> commands;
  commands["motd"] = &cmd_motd;
  commands["load"] = &cmd_load;
  commands["state"] = &cmd_state;
  commands["func"] = &cmd_func;
  commands["env"] = &cmd_env;
  commands["clear"] = &cmd_clear;
  commands["del"] = &cmd_del;
  commands["demo"] = &cmd_deutsch;

  while(true) {
    cout << ">>";
    string input;
    getline(cin, input);

    if(input[0] == '!') {
      //a special command
      size_t pos = input.find(' ');
      string cmd = input.substr(1, pos - 1);
      string arg = (pos == string::npos) ? "" : input.substr(pos + 1, string::npos);
      if(cmd == "quit") break;
      if(cmd == "help") {
        cout << "Commands:" << endl;
        cout << " !help" << endl;
        cout << " !quit" << endl;
        for(auto pair : commands) {
          cout << " !" << pair.first << endl;
        }
        continue;
      }

      if(commands.find(cmd) != commands.end()) {
        int result = commands[cmd](parser, p, e, arg);
        //0 means success, but don't print
        if(result == 1) {
          cout << "Command successful." << endl;
        } else if(result == -1) {
          cout << "Command unsuccessful." << endl;
        }
      } else {
        cout << "Invalid command." << endl;
      }
    } else {
      //a statement
      try {
        cout << "Result: " << parser.execute(input + ";", p, e) << endl;
      } catch(const QwakError& e) {
        cout << "Execution failed: " << e.what() << endl;
      } catch(...) {
        cout << "Execution failed." << endl;
      }
    }
  }

  cout << endl << "Bye!" << endl;

  return 0;
}