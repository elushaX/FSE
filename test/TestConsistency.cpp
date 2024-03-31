
#include "Interpreter.hpp"
#include <fstream>
#include <iostream>

// generates and saves final state to check if there are any logic changes to the code

int main(int argc, char* argv[]) {
  Interpreter interpreter;
  interpreter.logType = Interpreter::NONE;

  std::ifstream inputFile("commands");

  if (!inputFile.is_open()) {
    std::cerr << "Failed to open the file." << std::endl;
    return 1;
  }

  std::string line;
  while (std::getline(inputFile, line)) {
    interpreter.interpret(line);
  }

  inputFile.close();

  interpreter.dumpToFile("current_state");

  return 0;
}