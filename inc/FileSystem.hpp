#pragma once

#include "Path.hpp"
#include "DirectoryTree.hpp"

#include <sstream>

// Functionality:
// - print links and link counts
// - update link counts
// - add link creation commands

// Refactor
// - remove code duplication
// - Move DirectoryKey inside Node (no need to store pointers from Node to DirectoryTree::Node)
// - Merge DirectoryTree::Node and Node (no pointer overhead, requires tree nodes to be consistent, check insertNodeInstead)
// - introduce smart pointers
// - reconsider switch statements

class FileSystem {
public:
  FileSystem();
  ~FileSystem();

  bool makeDirectory(const Path& path);
  bool makeFile(const Path& path);
  bool changeCurrent(const Path& path);
  bool removeDirectory(const Path& path, bool recursively);
  bool removeFileOrLink(const Path& path);
  bool copyNode(const Path& source, const Path& to);
  bool moveNode(const Path& source, const Path& to);

  void log() const;

  const std::string& getLastError();

private:
  bool isPathContainsCurrent(Node* node);

private:
  Directory* root = nullptr;
  Directory* currentDirectory = nullptr;
};