#include "Interpreter.hpp"

#include <iostream>
#include <string>
#include <fstream>

int main(int argc, char* argv[]) {
  Interpreter interpreter;

  std::string filename = "commands";

  if (argc > 1) {
    filename = argv[1];
  }

  std::ifstream inputFile(filename);

  if (!inputFile.is_open()) {
    std::cerr << "Failed to open the file." << std::endl;
    return 1;
  }

  std::string line;
  while (std::getline(inputFile, line)) {
    std::cout << ">> " << line << std::endl;
    bool shouldStop = !interpreter.interpret(line);
    // if (shouldStop) break;
  }

  inputFile.close();

  return 0;
}