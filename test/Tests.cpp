
#include "Interpreter.hpp"
#include "UnitTest++/UnitTest++.h"

int main() {
  Interpreter interpreter;

  interpreter.interpret("");
  interpreter.interpret("m");
  interpreter.interpret("mD a");
  interpreter.interpret("mD /");
  interpreter.interpret("MD /A123");
  interpreter.interpret("cd /A123");
  interpreter.interpret("md asd");
  interpreter.interpret("md asd/asd");
  interpreter.interpret("md asd/asd/asd");
  interpreter.interpret("md asd/asd/asd/asd");
  interpreter.interpret("rd asd");

  return UnitTest::RunAllTests();
}