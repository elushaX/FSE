#include "Path.hpp"
#include <cassert>

std::vector<char> transitions;

void initializeTransitions() {
  transitions.resize(256);
  std::fill(transitions.begin(), transitions.end(), 1);
  for (char i = 'a'; i <= 'z'; i++) transitions[i] = i;
  for (char i = '0'; i <= '9'; i++) transitions[i] = i;
  for (char i = 'A'; i <= 'Z'; i++) transitions[i] = i /* + ('a' - 'A')*/;
  transitions['/'] = '/';
  transitions['.'] = '.';
}

Path::Path(const std::string& path) {
  set(path);
}

bool Path::isValid() const {
  return mIsValid;
}

bool Path::isInvalid() const {
  return !mIsValid;
}

bool Path::isAbsolute() const {
  return mAbsolute;
}

int Path::getDepth() const {
  return mChain.size();
}

const std::vector<std::string>& Path::getChain() const {
  return mChain;
}

std::vector<std::string> Path::getParentChain() const {
  std::vector<std::string> out = mChain;
  out.pop_back();
  return out;
}

const std::string& Path::getFilename() const {
  assert(getDepth());
  return mChain.back();
}

void Path::set(const std::string& path) {
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
  }
}

const std::string& Path::operator[](int idx) const {
  return mChain[idx];
}
