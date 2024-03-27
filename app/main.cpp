#include "Interpreter.hpp"

#include <iostream>
#include <string>

int main() {
  Interpreter interpreter;
  std::string line;

  std::cout << "File System emulator.\n";

  while (true) {
    std::cout << ">> ";
    std::getline(std::cin, line);

    if (line == "stop") {
      break;
    }

    // Process the line here
    //
    interpreter.interpret(line);
  }

  return 0;
}