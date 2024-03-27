
#include "FileSystem.hpp"

#include <iostream>

FileSystem::FileSystem() {
  root = new Directory();
  currentDirectory = root;
  initializeTransitions();
}

FileSystem::~FileSystem() {
  delete root;
}

bool FileSystem::makeDirectory(const Path& path) {
  if (path.getDepth() < 1 || path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  Directory* parentDirectory = path.isAbsolute() ? root : currentDirectory;

  auto existingNode = parentDirectory->findNode(path.getChain());
  if (existingNode) {
    if (existingNode->type == Node::DIRECTORY) return true;
    gError = "Cant create directory, such node exists";
    return false;
  }

  auto newDirectory = new Directory();
  if (!parentDirectory->attachNode(path.getParentChain(), path.getFilename(), newDirectory)) {
    delete newDirectory;
    gError = "Creation of intermediate directories is not supported";
    return false;
  }

  return true;
}

bool FileSystem::changeCurrent(const Path& path) {
  if (path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  auto node = path.isAbsolute() ? root->findNode(path.getChain(), 0) : currentDirectory->findNode(path.getChain(), 0);
  if (!node || node->type != Node::DIRECTORY) {
    gError = "No such directory";
    return false;
  }

  currentDirectory = (Directory*) node;
  return true;
}

void FileSystem::log() const {
  std::stringstream ss;
  std::vector<bool> indents;
  indents.resize(root->getMaxDepth());
  logNode(ss, root, 0, indents);
  std::cout << ss.str() << "\n";
}


void FileSystem::logNode(std::stringstream& ss, const Node* node, const Key& key, int depth, std::vector<bool>& indents) const {
  indent(ss, depth, indents);
  ss << key << "\n";
  
  switch (node->type) {
    case Node::DIRECTORY:
      return logDirectory(ss, (Directory*) node, depth, indents);
    case Node::FILE:
      return logFile(ss, (File*) node, depth, indents);
    case Node::LINK:
      return logLink(ss, (Link*) node, depth, indents);
  }
}

void FileSystem::indent(std::stringstream & ss, int depth, std::vector<bool>& indents) const {
  if (!depth) return;
  for (auto i  = 0; i < depth - 1; i++) {
    ss << (indents[i] ? " |" : "  ");
  }
  ss << " |_";
}

void FileSystem::logDirectory(std::stringstream & ss, const Directory* node, int depth, std::vector<bool>& indents) const {
  ss << (node == currentDirectory ? "* " : "  ");
  indent(ss, depth, indents);
  // ss << node->key << "\n";
  indents[depth] = true;
  depth++;
  /*
  auto lastNode = node->maxNode();
  node->traverseInorder([&](const Node* iterNode){
    if (lastNode == iterNode) indents[depth - 1] = false;
    logNode(ss, iterNode, depth, indents);
  });
  */
}

void FileSystem::logFile(std::stringstream& ss, const File* node, int depth, std::vector<bool>& indents) const {
  indent(ss, depth, indents);
  // ss << node->key << "\n";
}

void FileSystem::logLink(std::stringstream & ss, const Link* node, int depth, std::vector<bool>& indents) const {
  indent(ss, depth, indents);
  // ss << "link [" << node->key << "] \n";
}

const std::string& FileSystem::getLastError() {
  return gError;
}
