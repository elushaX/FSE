
#include "FileSystem.hpp"

#include "Directory.hpp"
#include "Link.hpp"

#include <iostream>
#include <cassert>
#include <algorithm>

static std::string gError;

FileSystem::FileSystem() {
  root = std::make_shared<Directory>();
  currentDirectory = root;
  initializeTransitions();
}

FileSystem::~FileSystem() = default;

std::shared_ptr<Node> FileSystem::getNode(const Path& path, bool parent) {
  auto pathChain = parent ? path.getParentChain() : path.getChain();
  auto currentNode = path.isAbsolute() ? root : currentDirectory;
  auto parentNode = pathChain.empty() ? currentNode : currentNode->findNode(pathChain);

  if (parent) {
    while (parentNode->getTarget()) {
      parentNode = parentNode->getTarget();
    }
  }

  return parentNode;
}

bool FileSystem::changeCurrent(const Path& path) {
  if (path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  auto parentNode = getNode(path, false);

  if (!parentNode) {
    gError = "No such directory";
    return false;
  }

  if (parentNode->getType() != Node::DIRECTORY) {
    gError = "Path is not a directory";
    return false;
  }

  currentDirectory = parentNode;
  return true;
}

bool FileSystem::makeDirectory(const Path& path) {
  if (path.getDepth() < 1 || path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  auto parentNode = getNode(path, true);

  if (!parentNode) {
    gError = "Invalid path";
    return false;
  }

  auto existingNode = parentNode->findNode(path.getFilename());
  if (existingNode) {
    if (!(existingNode->getType() == Node::DIRECTORY && existingNode->empty())) {
      gError = "File or link with such name already exists";
      return false;
    }
    return true;
  }

  auto directory = std::make_shared<Directory>();
  directory->mParent = parentNode;

  assert(parentNode->attachNode(path.getFilename(), directory));
  return true;
}

bool FileSystem::makeFile(const Path& path) {
  if (path.getDepth() < 1 || path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  auto parentNode = getNode(path, true);

  if (!parentNode) {
    gError = "Invalid path";
    return false;
  }

  auto existingNode = parentNode->findNode(path.getFilename());
  if (existingNode) {
    if (!(existingNode->getType() == Node::FILE && existingNode->empty())) {
      gError = "Directory with such name already exists";
      return false;
    }
    return true;
  }

  auto newFile = std::make_shared<Node>();
  newFile->mParent = parentNode;

  assert(parentNode->attachNode(path.getFilename(), newFile));
  return true;
}

bool FileSystem::makeLink(const Path& source, const Path& target, bool isDynamic) {
  if (source.getDepth() < 1 || source.isInvalid()) {
    gError = "Invalid source path";
    return false;
  }

  auto parentNodeSource = getNode(source, true);
  if (!parentNodeSource) {
    gError = "Invalid source path";
    return false;
  }

  auto parentNodeTarget = getNode(target, false);
  if (!parentNodeTarget) {
    gError = "Invalid target path";
    return false;
  }

  auto sourceNode = parentNodeSource->findNode(source.getFilename());
  if (!sourceNode) {
    gError = "Invalid source path";
    return false;
  }

  if (sourceNode->getType() == Node::LINK) {
    gError = "Link on link is not allowed";
    return false;
  }

  const Key& key = source.getFilename();

  if (parentNodeTarget->findNode(key)) {
    gError = "Node with such name already exists";
    return false;
  }

  auto newLink = std::make_shared<Link>();
  newLink->mParent = parentNodeTarget;

  if (!Link::linkNodes(newLink, sourceNode, !isDynamic)) {
    gError = "Can not link node";
    return false;
  }

  assert(parentNodeTarget->attachNode(key, newLink));
  return true;
}

bool FileSystem::removeDirectory(const Path& path, bool recursively) {
  if (path.getDepth() < 1 || path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  auto parentNode = getNode(path, true);

  if (!parentNode) {
    gError = "Invalid path";
    return false;
  }

  auto existingNode = parentNode->findNode(path.getFilename());
  if (!existingNode) {
    gError = "Directory with such name does not exist";
    return false;
  }

  if (existingNode->getType() != Node::DIRECTORY) {
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

  existingNode->clearFlags(parentNode);
  if (existingNode->isHard()) {
    gError = "Directory is referenced by a hard links";
    return false;
  }

  // deletes dynamic incoming links to this file system sub-ree
  existingNode->removeIncomingDynamicLinks();

  if (!parentNode->detachNode(path.getFilename())) {
    gError = "Can not remove directory because it has hard references";
    return false;
  }

  return true;
}

bool FileSystem::removeFileOrLink(const Path &path) {
  if (path.getDepth() < 1 || path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  auto parentNode = getNode(path, true);

  if (!parentNode) {
    gError = "Invalid path";
    return false;
  }

  auto existingNode = parentNode->findNode(path.getFilename());
  if (!existingNode) {
    gError = "File or link with such name does not exist";
    return false;
  }

  if (existingNode->getType() == Node::DIRECTORY) {
    gError = "Path is not a file or a link";
    return false;
  }

  existingNode->clearFlags(parentNode);
  if (existingNode->isHard()) {
    gError = "File is referenced by a hard link";
    return false;
  }

  if (auto link = dynamic_pointer_cast<Link>(existingNode)) {
    Link::unlinkNodes(link);
  }

  if (!parentNode->detachNode(path.getFilename())) {
    gError = "Can not remove file or link because it is referenced by a hard link";
    return false;
  }

  return true;
}

bool FileSystem::copyNode(const Path& source, const Path& target) {
  if (source.getDepth() < 1 || source.isInvalid()) {
    gError = "Invalid source path";
    return false;
  }

  auto parentNodeSource = getNode(source, true);
  if (!parentNodeSource) {
    gError = "Invalid source path";
    return false;
  }

  auto parentNodeTarget = getNode(target, false);
  if (!parentNodeTarget) {
    gError = "Invalid target path";
    return false;
  }

  auto sourceNode = parentNodeSource->findNode(source.getFilename());
  if (!sourceNode) {
    gError = "Invalid source path";
    return false;
  };

  if (parentNodeTarget->getType() != Node::DIRECTORY) {
    gError = "Target path is not a directory";
    return false;
  }

  Key key = source.getFilename();
  while (parentNodeTarget->findNode(key)) {
    // gError = "Node with such name already exists in the target directory";
    // return false;
    key += "_copy";
  }

  auto clonedNode = sourceNode->clone();
  clonedNode->mParent = parentNodeTarget;
  assert(parentNodeTarget->attachNode(key, clonedNode));

  return true;
}

bool FileSystem::moveNode(const Path &source, const Path &target) {
  if (source.getDepth() < 1 || source.isInvalid()) {
    gError = "Invalid source path";
    return false;
  }

  auto parentNodeSource = getNode(source, true);
  if (!parentNodeSource) {
    gError = "Invalid source path";
    return false;
  }

  auto parentNodeTarget = getNode(target, false);
  if (!parentNodeTarget) {
    gError = "Invalid target path";
    return false;
  }

  auto sourceNode = parentNodeSource->findNode(source.getFilename());
  if (!sourceNode) {
    gError = "Invalid source path";
    return false;
  }

  const Key& key = source.getFilename();

  if (parentNodeTarget->findNode(key)) {
    gError = "Node with such name already exists in the target directory";
    return false;
  }

  sourceNode->clearFlags(parentNodeSource);
  if (sourceNode->isHard()) {
    gError = "Node is referenced by a hard link";
    return false;
  }

  assert(parentNodeTarget->attachNode(key, sourceNode));
  assert(parentNodeSource->detachNode(key));

  sourceNode->mParent = parentNodeTarget;
  return true;
}

void FileSystem::log() const {
  std::stringstream ss;

  std::vector<std::shared_ptr<Node>> currentPath;
  root->getNodeStraightPath(currentDirectory, currentPath);
  std::reverse(currentPath.begin(), currentPath.end());

  ss << "cd [";
  for (const auto& node : currentPath) {
    ss << node->mKey << "/";
  }
  ss << "]\n";

  root->dump(ss);
  std::cout << ss.str();
}

const std::string& FileSystem::getLastError() {
  return gError;
}

bool FileSystem::isPathContainsCurrent(const std::shared_ptr<Node>& node) {
  std::vector<std::shared_ptr<Node>> currentPath;
  std::vector<std::shared_ptr<Node>> path;

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