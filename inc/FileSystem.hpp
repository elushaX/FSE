#pragma once

#include "Node.hpp"
#include "Path.hpp"

#include <sstream>

// tests
// links have unique names
// use C:/ for root
// copy operator exit if exists, do not rename
// clean-ups
// in subtree links should be in-tree after copy operation
// update link if exists

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
  bool isPathContains(const std::shared_ptr<Node>& node, const std::shared_ptr<Node>& contains);
  std::shared_ptr<Node> getNode(const Path& path, bool parent);

private:
  std::shared_ptr<Node> root = nullptr;
  std::shared_ptr<Node> currentDirectory = nullptr;
};