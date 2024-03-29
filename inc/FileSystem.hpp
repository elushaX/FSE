#pragma once

#include "Path.hpp"
#include "DirectoryTree.hpp"

#include <sstream>

// FIX MEMORY VIOLATIONS WITH LINKS


// DONT STORE CACHE
// DO O(N) deletion and moving
// in-tree node links will report false to those operations and hard nodes deletion
// use in node 'is_delete' flag and travers all nodes with link checks
// remove mParent mTreeNode links

// Key (is copied on each tree access) !!!

// Functionality:
// deleting directory - mark all nodes as deleted
// traverse and check for links
// unlink if those links are outgoing

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
  bool makeLink(const Path& source, const Path& to, bool isDynamic);

  void log() const;

  static const std::string& getLastError();

private:
  bool isPathContainsCurrent(Node* node);

private:
  Directory* root = nullptr;
  Directory* currentDirectory = nullptr;
};