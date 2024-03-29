
#include "FileSystem.hpp"

#include "Directory.hpp"
#include "Link.hpp"

#include <iostream>
#include <cassert>
#include <algorithm>

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

  Node* parentNode = path.isAbsolute() ? root : currentDirectory;

  auto newDirectory = new Directory();
  if (!parentNode->attachNode(path.getParentChain(), path.getFilename(), newDirectory)) {
    delete newDirectory;
    gError = "Invalid path or node exists";
    return false;
  }

  return true;
}

bool FileSystem::makeFile(const Path& path) {
  if (path.getDepth() < 1 || path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  Node* parentNode = path.isAbsolute() ? root : currentDirectory;

  auto newFile = new Node();
  if (!parentNode->attachNode(path.getParentChain(), path.getFilename(), newFile)) {
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

  Node* parentNode = path.isAbsolute() ? root : currentDirectory;

  auto existingNode = parentNode->findNode(path.getChain());
  if (!existingNode) {
    gError = "Cant remove, such node doesnt exists";
    return false;
  }

  if (!existingNode->isDirectory()) {
    gError = "Path is not a directory";
    return false;
  }

  if (!recursively && !existingNode->empty()) {
    gError = "Directory is not empty";
    return false;
  }

  if (isPathContainsCurrent(existingNode)) {
    gError = "Can not remove directory you are currently working in";
    return false;
  }

  if (!parentNode->detachNode(path.getParentChain(), path.getFilename())) {
    gError = "Can not detach node";
    return false;
  }

  delete existingNode;
  return true;
}

bool FileSystem::copyNode(const Path& source, const Path& target) {
  if (source.getDepth() < 1 || source.isInvalid()) {
    gError = "Invalid source path";
    return false;
  }

  Node* workingDirectorySource = source.isAbsolute() ? root : currentDirectory;
  Node* workingDirectoryTarget = target.isAbsolute() ? root : currentDirectory;

  auto sourceNode = workingDirectorySource->findNode(source.getChain());
  if (!sourceNode) {
    gError = "Invalid source path";
    return false;
  }

  auto targetNode = workingDirectoryTarget->findNode(target.getChain());
  if (!targetNode) {
    gError = "Invalid target directory";
    return false;
  }

  Key key = source.getFilename();

  if (targetNode->findNode(key)) {
    // gError = "Node with such name already exists in the target directory";
    // return false;
    key += "_copy";
  }

  Node* clonedNode = sourceNode->clone();

  if (!targetNode->attachNode(key, clonedNode)) {
    delete clonedNode;
    return false;
  }

  return true;
}

bool FileSystem::moveNode(const Path &source, const Path &target) {
  if (source.getDepth() < 1 || source.isInvalid()) {
    gError = "Invalid source path";
    return false;
  }

  Node* workingDirectorySource = source.isAbsolute() ? root : currentDirectory;
  Node* workingDirectoryTarget = target.isAbsolute() ? root : currentDirectory;

  auto sourceParentNode = workingDirectorySource->findNode(source.getParentChain());
  if (!sourceParentNode) {
    gError = "Invalid source path";
    return false;
  }

  auto sourceNode = sourceParentNode->findNode(source.getFilename());
  if (!sourceNode) {
    gError = "Invalid source path";
    return false;
  }

  auto targetNode = workingDirectoryTarget->findNode(target.getChain());
  if (!targetNode) {
    gError = "Invalid target directory";
    return false;
  }

  const Key& key = source.getFilename();

  if (sourceParentNode->isHard()) {
    gError = "Node contains incoming hard links";
    return false;
  }

  if (!targetNode->attachNode(key, sourceNode)) {
    return false;
  }

  assert(sourceParentNode->detachNode(key));

  return true;
}

bool FileSystem::makeLink(const Path& source, const Path& target, bool isDynamic) {
  if (source.getDepth() < 1 || source.isInvalid()) {
    gError = "Invalid source path";
    return false;
  }

  Node* workingDirectorySource = source.isAbsolute() ? root : currentDirectory;
  Node* workingDirectoryTarget = target.isAbsolute() ? root : currentDirectory;

  auto sourceNode = workingDirectorySource->findNode(source.getChain());
  if (!sourceNode) {
    gError = "Invalid source path";
    return false;
  }

  if (sourceNode->isLink()) {
    gError = "Link on link is not allowed";
    return false;
  }

  auto targetNode = workingDirectoryTarget->findNode(target.getChain());
  if (!targetNode || !targetNode->isDirectory()) {
    gError = "Invalid target directory";
    return false;
  }

  const Key& key = source.getFilename();
  auto targetDirectory = (Directory*)targetNode;

  auto newLink = new Link(sourceNode, !isDynamic);
  if (!targetDirectory->attachNode(key, newLink)) {
    delete newLink;
    return false;
  }

  return true;
}

bool FileSystem::removeFileOrLink(const Path &path) {
  if (path.getDepth() < 1 || path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  Node* parentDirectory = path.isAbsolute() ? root : currentDirectory;

  auto existingNode = parentDirectory->findNode(path.getChain());
  if (!existingNode) {
    gError = "Cant remove, such node doesnt exists";
    return false;
  }

  if (existingNode->isDirectory()) {
    gError = "Path is a directory";
    return false;
  }

  if (!parentDirectory->detachNode(path.getParentChain(), path.getFilename())) {
    gError = "Can not remove node";
    return false;
  }

  delete existingNode;
  return true;
}

bool FileSystem::changeCurrent(const Path& path) {
  if (path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  auto node = path.isAbsolute() ? root->findNode(path.getChain(), 0) : currentDirectory->findNode(path.getChain(), 0);
  if (!node || !node->isDirectory()) {
    gError = "No such directory";
    return false;
  }

  currentDirectory = node;
  return true;
}

void FileSystem::log() const {
  std::stringstream ss;

  std::vector<const Node*> currentPath;
  root->getNodeStraightPath(currentDirectory, currentPath);
  std::reverse(currentPath.begin(), currentPath.end());

  ss << "cd - /";
  for (auto key : currentPath) {
    ss << "X" << "/";
  }

  ss << "\n";
  root->dump(ss);
  std::cout << ss.str();
}

const std::string& FileSystem::getLastError() {
  return gError;
}

bool FileSystem::isPathContainsCurrent(Node* node) {
  std::vector<const Node*> currentPath;
  std::vector<const Node*> path;

  root->getNodeStraightPath(currentDirectory, currentPath);
  root->getNodeStraightPath(node, path);

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