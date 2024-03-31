
#pragma once

#include "Interpreter.hpp"
#include "UnitTest++/UnitTest++.h"

#include <random>
#include <chrono>

const ui64 timeLimitSec = 1;
const ui64 itemLimit = 1e6;

std::random_device rd;
std::mt19937 rng(rd());
// std::mt19937 rng;

class StringDistribution {
private:
  std::vector<std::string> commandList;
  std::vector<double> probabilities;
  std::discrete_distribution<int> distribution;

public:
  explicit StringDistribution(const std::map<std::string, double>& commands) {
    for (const auto& command : commands) {
      commandList.push_back(command.first);
      probabilities.push_back(command.second);
    }
    double sum = std::accumulate(probabilities.begin(), probabilities.end(), 0.0);
    std::for_each(probabilities.begin(), probabilities.end(), [sum](double& element) { element /= sum; });
    distribution = std::discrete_distribution<int>(probabilities.begin(), probabilities.end());
  }

  std::string get() {
    return commandList[distribution(rng)];
  }
};

template<typename tDriver>
void runFor(Interpreter& interpreter, ui64 time, ui64 size, tDriver driver) {
  auto startTime = std::chrono::steady_clock::now();
  while (true) {
    auto currentTime = std::chrono::steady_clock::now();
    double elapsedSeconds = std::chrono::duration<double>(currentTime - startTime).count();
    if (elapsedSeconds >= (double) time || interpreter.mFileSystem.size() > size) break;
    interpreter.interpret(driver());
  }
}