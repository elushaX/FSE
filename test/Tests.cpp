
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
  interpreter.interpret("deltree A123-copy");

  interpreter.interpret("md a");
  interpreter.interpret("md a/b");
  interpreter.interpret("md a/b/c");
  interpreter.interpret("mhl a /a/b/c/");
  interpreter.interpret("md a/b/c/a/k");
  interpreter.interpret("copy a /");

  interpreter.interpret("mdl a/b/c /a-copy/k");

  interpreter.interpret("move a/b/c /a-copy");

  interpreter.interpret("del /a-copy/k/c");

  return UnitTest::RunAllTests();
}