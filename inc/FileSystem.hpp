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
  void log() const;

  const std::string& getLastError();

private:
  void logNode(std::stringstream& ss, const Node* node, int depth, std::vector<bool>& indents) const;
  void indent(std::stringstream & ss, int depth, std::vector<bool>& indents) const;
  void logDirectory(std::stringstream & ss, const Directory* node, int depth, std::vector<bool>& indents) const;
  void logFile(std::stringstream& ss, const File* node, int depth, std::vector<bool>& indents) const;
  void logLink(std::stringstream & ss, const Link* node, int depth, std::vector<bool>& indents) const;

private:
  Directory* root = nullptr;
  Directory* currentDirectory = nullptr;
};