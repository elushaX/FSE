#pragma once

#include <vector>
#include <string>

typedef unsigned long long ui64;

extern std::vector<char> transitions;
void initializeTransitions();

class Path {
public:
  Path() = default;
  explicit Path(const std::string& path);

  void set(const std::string& path);
  const std::string& operator[](int idx) const;

  [[nodiscard]] bool isInvalid() const;
  [[nodiscard]] bool isAbsolute() const;
  [[nodiscard]] ui64 getDepth() const;

  [[nodiscard]] const std::vector<std::string>& getChain() const;
  [[nodiscard]] std::vector<std::string> getParentChain() const;
  [[nodiscard]] const std::string& getFilename() const;

private:
  std::vector<std::string> mChain;
  bool mAbsolute = false;
  bool mIsValid = false;
  bool mDirectory = false;
};