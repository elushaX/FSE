#include "Path.hpp"
#include <cassert>
#include <cstring>

std::vector<char> transitions;
constexpr auto drivePrefix = "C:";

void initializeTransitions() {
  transitions.resize(256);
  std::fill(transitions.begin(), transitions.end(), 1);
  for (char i = 'a'; i <= 'z'; i++) transitions[i] = i;
  for (char i = '0'; i <= '9'; i++) transitions[i] = i;
  for (char i = 'A'; i <= 'Z'; i++) transitions[i] = i /* + ('a' - 'A')*/;
  transitions['/'] = '/';
  transitions['.'] = '.';
  transitions['_'] = '_';
}

Path::Path(const std::string& path) {
  set(path);
}

bool Path::isInvalid() const {
  return !mIsValid;
}

bool Path::isAbsolute() const {
  return mAbsolute;
}

ui64 Path::getDepth() const {
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
  mIsValid = true;

  if (path.empty()) {
    mIsValid = false;
    return;
  }

  std::string lowercasePath = path;

  const auto drivePrefixSize = std::strlen(drivePrefix);
  assert(drivePrefixSize);

  if ((lowercasePath.size() >= drivePrefixSize) && (std::memcmp(lowercasePath.c_str(), drivePrefix, drivePrefixSize) == 0)) {
    lowercasePath.erase(0, drivePrefixSize);
    mAbsolute = true;
    if (lowercasePath.empty()) lowercasePath = "/";
    else if (lowercasePath[0] != '/') {
      mIsValid = false;
      return;
    }
  } else {
    if (lowercasePath[0] == '/') {
      mIsValid = false;
      return;
    }
  }


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
