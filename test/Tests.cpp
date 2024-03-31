
#include <iostream>
#include <fstream>
#include "Tests.hpp"

SUITE(FSE) {
  TEST (Simple) {
    Interpreter interpreter;
    auto interp = [&](auto name) {
      return interpreter.interpret(name);
    };

    CHECK(interp("md a"));
    CHECK(interp("md a/b"));
    CHECK(interp("mdl a a/b"));
    CHECK(interp("copy a C:/"));
    CHECK(interp("deltree a"));
    CHECK(interp("deltree a_copy"));

    CHECK(interp("md a"));
    CHECK(interp("md a/b"));
    CHECK(interp("mhl a a/b"));
    CHECK(interp("copy a C:/"));
    CHECK(!interp("deltree a"));
    CHECK(interp("deltree a_copy"));
    CHECK(interp("del a/b/a"));
    CHECK(interp("deltree a"));

    CHECK(interp("md a"));
    CHECK(interp("md a/b"));
    CHECK(interp("mdl a a/b"));
    CHECK(interp("copy a C:/"));
    CHECK(interp("md c"));
    CHECK(interp("move a c"));
    CHECK(interp("move a_copy c"));

    CHECK(interp("copy c C:/"));

    CHECK(interp("deltree c"));
  }

  TEST (CommandNoise) {
    Interpreter interpreter;
    interpreter.logType = Interpreter::NONE;

    StringDistribution commands({
        { "md", 10 },
        { "mf", 10 },
        { "mhl", 20 },
        { "mdl", 20 },
        { "cd", 5 },
        { "rd", 5 },
        { "del", 1 },
        { "deltree", 2 },
        { "move", 20 },
        { "copy", 5 },
    });

    StringDistribution names({
        { "C:", 1 },
        { "a", 1 },
        { "b", 1 },
        { "c", 1 },
        { "d", 1 },
        { "e", 1 },
        { "f", 1 },
        { "g", 1 },
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

    // std::ofstream commandsDump("consistency_commands_new");

    runFor(interpreter, timeLimitSec, itemLimit, [&]() {
      std::stringstream ss;
      ss << commands.get();
      generatePath(ss);
      if (pathLenDist(rng) % 2) generatePath(ss);
      // commandsDump << ss.str() << "\n";
      return ss.str();
    });

    // interpreter.dumpToFile("consistency_state_new");
  }
}

int main() {
  return UnitTest::RunAllTests();
}