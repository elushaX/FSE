#pragma once

#include "Node.hpp"
#include "Path.hpp"

#include <sstream>

// copy operator exit if exists, do not rename
// links have unique names
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

  std::ostream& dump(std::ostream& stream) const;
  void log() const;
  [[nodiscard]] ui64 size() const;

  static const std::string& getLastError();

private:
  bool isPathContains(const std::shared_ptr<Node>& node, const std::shared_ptr<Node>& contains);
  std::shared_ptr<Node> getNode(const Path& path, bool parent);

private:
  std::shared_ptr<Node> root = nullptr;
  std::shared_ptr<Node> currentDirectory = nullptr;
};