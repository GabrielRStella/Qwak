#include "qwak.h"

#include <iostream>
#include <unordered_map>

using namespace Qwak;

using namespace std;

/*
TODO:
-commands
--!load: load a .qwak file
--!env: print all variables in environment
--!func: print all functions in program
-?
*/

typedef int (*CMD)(QwakParser&, Program&, Environment&, const string&);

int cmd_motd(QwakParser& parser, Program& p, Environment& e, const std::string& args) {
  cout << "MOTD: " << parser.motd() << endl;
  return 0;
}

int cmd_load(QwakParser& parser, Program& p, Environment& e, const std::string& args) {
  cout << "Loading \"" << args << "\"..." << endl;
  return -1;
}

int cmd_state(QwakParser& parser, Program& p, Environment& e, const std::string& args) {
  cout << "State: " << e.getState() << endl;
  return 0;
}

int main() {

  QwakParser parser;

  Program* pr = parser.createEmptyProgram();
  Program& p = *pr;

  Environment e;

  cout << "Environment created! Running QWAK " << parser.version() << ": \"" << parser.motd() << "\"" << endl;

  unordered_map<std::string, CMD> commands;
  commands["motd"] = &cmd_motd;
  commands["load"] = &cmd_load;
  commands["state"] = &cmd_state;

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
        cout << "Result: " << parser.execute(input, p, e) << endl;
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