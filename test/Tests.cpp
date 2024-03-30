
#include "Interpreter.hpp"
#include "UnitTest++/UnitTest++.h"

int main() {
  Interpreter interpreter;
  interpreter.interpret("");
  return UnitTest::RunAllTests();
}