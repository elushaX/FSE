
#include "Interpreter.hpp"
#include "UnitTest++/UnitTest++.h"

int main() {
  Interpreter interpreter;
  interpreter.interpret("md a");
  interpreter.interpret("md a/b");
  interpreter.interpret("mdl a a/b");
  interpreter.interpret("copy a C:/");
  interpreter.interpret("deltree a");
  interpreter.interpret("deltree a_copy");


  interpreter.interpret("md a");
  interpreter.interpret("md a/b");
  interpreter.interpret("mhl a a/b");
  interpreter.interpret("copy a C:/");
  interpreter.interpret("deltree a");
  interpreter.interpret("deltree a_copy");
  interpreter.interpret("del a/b/a");
  interpreter.interpret("deltree a");

  interpreter.interpret("md a");
  interpreter.interpret("md a/b");
  interpreter.interpret("mdl a a/b");
  interpreter.interpret("copy a C:/");
  interpreter.interpret("md c");
  interpreter.interpret("move a c");
  interpreter.interpret("move a_copy c");

  interpreter.interpret("copy c C:/");

  interpreter.interpret("deltree c");

  return UnitTest::RunAllTests();
}