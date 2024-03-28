#pragma once

#include "Path.hpp"
#include "DirectoryTree.hpp"

#include <sstream>

// Functionality:
// - add node to path conversion
// - check for current node deletion
// - print current node
// - print links and link counts
// - update link counts
// - add link creation commands

// Refactor
// - Move DirectoryKey inside Node (no need to store pointers from Node to DirectoryTree::Node)
// - Merge DirectoryTree::Node and Node (no pointer overhead, requires tree nodes to be consistent, check insertNodeInstead)
// - introduce smart pointers
// - reconsider switch statements

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
  bool isPathContainsCurrent(Node* node);

private:
  Directory* root = nullptr;
  Directory* currentDirectory = nullptr;
};