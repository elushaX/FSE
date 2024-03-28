
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
    gError = "Can not create directory, such node exists";
    return false;
  }

  auto newDirectory = new Directory();
  if (!parentDirectory->attachNode(path.getParentChain(), path.getFilename(), newDirectory)) {
    delete newDirectory;
    gError = "Invalid path";
    return false;
  }

  return true;
}

bool FileSystem::makeFile(const Path& path) {
  if (path.getDepth() < 1 || path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  Directory* parentDirectory = path.isAbsolute() ? root : currentDirectory;

  auto existingNode = parentDirectory->findNode(path.getChain());
  if (existingNode) {
    if (existingNode->mType == Node::FILE) return true;
    gError = "Can not create file, such node exists";
    return false;
  }

  auto newFile = new File();
  if (!parentDirectory->attachNode(path.getParentChain(), path.getFilename(), newFile)) {
    delete newFile;
    gError = "Invalid path";
    return false;
  }

  return true;
}

bool FileSystem::removeDirectory(const Path& path, bool recursively) {
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

  if (existingNode->mType != Node::DIRECTORY) {
    gError = "Path is not a directory";
    return false;
  }

  if (!recursively && existingNode->mType == Node::DIRECTORY && ((Directory*)existingNode)->size()) {
    gError = "Directory is not empty";
    return false;
  }

  if (isPathContainsCurrent(existingNode)) {
    gError = "Can not remove directory you are currently working in";
    return false;
  }

  assert(parentDirectory->detachNode(path.getParentChain(), path.getFilename()));

  delete existingNode;
  return true;
}

bool FileSystem::removeFileOrLink(const Path &path) {
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

  if (existingNode->mType == Node::DIRECTORY) {
    gError = "Path is a directory";
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

bool FileSystem::isPathContainsCurrent(Node* node) {
  std::vector<const Key*> currentPath;
  std::vector<const Key*> path;

  root->getNodePath(currentDirectory, currentPath);
  root->getNodePath(node, path);

  std::reverse(currentPath.begin(), currentPath.end());
  std::reverse(path.begin(), path.end());

  if (path.size() > currentPath.size()) {
    return false;
  }

  for (ui32 i = 0; i < path.size(); i++) {
    if (path[i] != currentPath[i]) return false;
  }

  return true;
}