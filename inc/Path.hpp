#pragma once

#include <vector>
#include <string>

extern std::vector<char> transitions;
void initializeTransitions();

class Path {
public:
  Path() = default;
  Path(const char* path);

  void set(const std::string& path);
  const std::string& operator[](int idx) const;

  bool isValid() const;
  bool isInvalid() const;
  bool isAbsolute() const;
  int getDepth() const;

  const std::vector<std::string>& getChain() const;
  std::vector<std::string> getParentChain() const;
  const std::string& getFilename() const;

private:
  std::vector<std::string> mChain;
  bool mAbsolute = false;
  bool mIsValid = false;
  bool mDirectory = false;
};