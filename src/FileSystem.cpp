
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
    if (existingNode->mType == Node::DIRECTORY) return true;
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

bool FileSystem::removeDirectory(const Path& path) {
  if (path.getDepth() < 1 || path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  Directory* parentDirectory = path.isAbsolute() ? root : currentDirectory;

  auto existingNode = parentDirectory->findNode(path.getChain());
  if (!existingNode) {
    gError = "Cant remove, such node doesnt exists";
    return false;
  }

  assert(parentDirectory->detachNode(path.getParentChain(), path.getFilename()));

  delete existingNode;
  return true;
}

bool FileSystem::changeCurrent(const Path& path) {
  if (path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  auto node = path.isAbsolute() ? root->findNode(path.getChain(), 0) : currentDirectory->findNode(path.getChain(), 0);
  if (!node || node->mType != Node::DIRECTORY) {
    gError = "No such directory";
    return false;
  }

  currentDirectory = (Directory*) node;
  return true;
}

void FileSystem::log() const {
  std::stringstream ss;

  std::vector<const Key*> currentPath;
  root->getNodePath(currentDirectory, currentPath);
  std::reverse(currentPath.begin(), currentPath.end());

  ss << "cd - /";
  for (auto key : currentPath) {
    ss << *key << "/";
  }

  ss << "\n";
  root->dump(ss);
  std::cout << ss.str();
}

const std::string& FileSystem::getLastError() {
  return gError;
}
