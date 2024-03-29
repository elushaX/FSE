#pragma once

#include "Node.hpp"
#include "Path.hpp"

#include <sstream>

// restore path unwindingk
// RESTORE LINKS
// COPY operator update link targets on copied nodes

// use in node 'is_delete' flag and travers all nodes with link checks
// deleting directory - mark all nodes as deleted
// traverse and check for links
// unlink if those links are outgoing


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
  bool isPathContainsCurrent(const std::shared_ptr<Node>& node);
  std::shared_ptr<Node> getNode(const Path& path, bool parent);

private:
  std::shared_ptr<Node> root = nullptr;
  std::shared_ptr<Node> currentDirectory = nullptr;
};