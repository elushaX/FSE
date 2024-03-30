
#include <iostream>
#include "Tests.hpp"

SUITE(FSE) {
  TEST (Simple) {
    Interpreter interpreter;
    auto interp = [&](auto name) {
      interpreter.interpret(name);
    };

    interp("md a");
    interp("md a/b");
    interp("mdl a a/b");
    interp("copy a C:/");
    interp("deltree a");
    interp("deltree a_copy");

    interp("md a");
    interp("md a/b");
    interp("mhl a a/b");
    interp("copy a C:/");
    interp("deltree a");
    interp("deltree a_copy");
    interp("del a/b/a");
    interp("deltree a");

    interp("md a");
    interp("md a/b");
    interp("mdl a a/b");
    interp("copy a C:/");
    interp("md c");
    interp("move a c");
    interp("move a_copy c");

    interp("copy c C:/");

    interp("deltree c");
  }

  TEST (CommandNoise) {
    Interpreter interpreter;

    StringDistribution commands({
        { "md", 10 },
        { "mf", 10 },
        { "mhl", 10 },
        { "mdl", 10 },
        { "cd", 5 },
        { "rd", 1 },
        { "del", 10 },
        { "deltree", 10 },
        { "move", 20 },
        { "copy", 20 },
    });

    StringDistribution names({
        { "C:", 1 },
        { "a", 1 },
        { "b", 1 },
        { "c", 1 },
        { "d", 1 },
        { "e", 1 },
    });

    std::discrete_distribution<int> pathLenDist({ 0.1, 0.2, 0.4, 0.5, 0.1 });

    auto generatePath = [&](std::stringstream& ss) {
      ss << " ";
      auto len = pathLenDist(rng);
      if (len) ss << names.get();
      for (auto i = 1; i < len; i++) {
        ss <<"/" << names.get();
      }
    };

    runFor(interpreter, timeLimitSec, itemLimit, [&]() {
      std::stringstream ss;
      ss << commands.get();
      generatePath(ss);
      if (pathLenDist(rng) % 2) generatePath(ss);
      return ss.str();
    });

    interpreter.mFileSystem.log();
  }
}

int main() {
  return UnitTest::RunAllTests();
}