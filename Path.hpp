#pragma once

#include <algorithm>
#include <vector>
#include <string>

std::vector<char> transitions;

void initializeTransitions() {
  transitions.resize(256);
  std::fill(transitions.begin(), transitions.end(), 1);
  for (char i = 'a'; i <= 'z'; i++) transitions[i] = i;
  for (char i = '0'; i <= '9'; i++) transitions[i] = i;
  for (char i = 'A'; i <= 'Z'; i++) transitions[i] = i + ('a' - 'A');
  transitions['/'] = '/';
}

char getChar(char in) {
  return transitions[in];
}

class Path {
public:
  Path() = default;

  Path(const char* path) {
    set(path);
  }

  bool isValid() {
    return mIsValid;
  }

  bool isAbsolute() const {
    return mAbsolute;
  }

  int getDepth() const {
    return mDepth;
  }

  void set(const std::string& path) {
    if (path.empty()) {
      mIsValid = false;
      return;
    }

    mIsValid = true;
    mPath = path;

    mDepth = std::count(path.begin(), path.end(), '/') + 1;
    mDepth -= (mPath.back() == '/') + (path.front() == '/');

    mAbsolute = path.front() == '/';

    for (auto & character : mPath) {
      character = getChar(character);
      if (character == 1) mIsValid = false;
    }
  }

private:
  std::string mPath = "";
  int mDepth = 0;
  bool mAbsolute = false;
  bool mIsValid = false;
};