#pragma once

#include "Path.hpp"
#include "DirectoryTree.hpp"

#include <sstream>

class FileSystem {
public:
  FileSystem();
  ~FileSystem();

  bool makeDirectory(const Path& path);
  bool changeCurrent(const Path& path);
  bool removeDirectory(const Path& path);

  void log() const;

  const std::string& getLastError();

private:
  Directory* root = nullptr;
  Directory* currentDirectory = nullptr;
};