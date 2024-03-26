#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cassert>

std::vector<char> transitions;

void initializeTransitions() {
  transitions.resize(256);
  std::fill(transitions.begin(), transitions.end(), 1);
  for (char i = 'a'; i <= 'z'; i++) transitions[i] = i;
  for (char i = '0'; i <= '9'; i++) transitions[i] = i;
  for (char i = 'A'; i <= 'Z'; i++) transitions[i] = i + ('a' - 'A');
  transitions['/'] = '/';
  transitions['.'] = '.';
}

class Path {
public:
  Path() = default;

  Path(const char* path) {
    set(path);
  }

  bool isValid() const {
    return mIsValid;
  }

  bool isInvalid() const {
    return !mIsValid;
  }

  bool isAbsolute() const {
    return mAbsolute;
  }

  int getDepth() const {
    return mChain.size();
  }

  const std::vector<std::string>& getChain() const {
    return mChain;
  }

  std::vector<std::string> getParentChain() const {
    std::vector<std::string> out = mChain;
    out.pop_back();
    return out;
  }

  const std::string& getFilename() const {
    assert(getDepth());
    return mChain.back();
  }

  void set(const std::string& path) {
    if (path.empty()) {
      mIsValid = false;
      return;
    }

    std::string lowercasePath = path;
    mIsValid = true;
    mAbsolute = path.front() == '/';
    mDirectory = path.back() == '/';
    mChain.clear();

    for (auto & character : lowercasePath) {
      character = transitions[character];
      if (character == 1) mIsValid = false;
    }

    if (lowercasePath == "/") return;

    const char* begin = &lowercasePath.front() + (lowercasePath.front() == '/');
    const char* end = &lowercasePath.back() - (lowercasePath.back() == '/');
    const char* prev = begin;

    for (const char* iter = begin; iter <= end; iter++) {
      if (*iter == '/')  {
        const auto string = lowercasePath.substr(prev - lowercasePath.c_str(), iter - prev);
        mChain.push_back(string);
        prev = iter + 1;
      }
    }

    const auto string = lowercasePath.substr(prev - lowercasePath.c_str(), end - prev + 1);
    mChain.push_back(string);

    for (auto & key : mChain) {
      if (key.empty()) mIsValid = false;
      // std::cout << key << ' ';
    }
    // std::cout << "\n";
  }

  const std::string& operator[](int idx) const {
    return mChain[idx];
  };

private:
  std::vector<std::string> mChain;
  bool mAbsolute = false;
  bool mIsValid = false;
  bool mDirectory = false;
};