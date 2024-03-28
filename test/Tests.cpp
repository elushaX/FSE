
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
  interpreter.interpret("md asd/b");
  interpreter.interpret("md asd/b/c");
  interpreter.interpret("md asd/b/c/d");

  interpreter.interpret("cd asd/b/c/d");

  interpreter.interpret("cd /");

  interpreter.interpret("copy A123 / ");

  interpreter.interpret("rd d");
  interpreter.interpret("deltree A123");

  return UnitTest::RunAllTests();
}