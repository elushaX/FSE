
#include "Interpreter.hpp"

int main() {
  Interpreter interpreter;

  interpreter.interpret("");
  interpreter.interpret("m");
  interpreter.interpret("mD a");
  interpreter.interpret("mD /");
  interpreter.interpret("MD /A123");
  interpreter.interpret("cd /A123");
  interpreter.interpret("md asd");


  return 0;
}