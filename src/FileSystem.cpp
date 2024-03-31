
#include "FileSystem.hpp"

#include "Directory.hpp"
#include "Link.hpp"

#include <iostream>
#include <cassert>
#include <algorithm>

static std::string gError;

FileSystem::FileSystem() {
  root = std::make_shared<Directory>();
  root->mKey = "C:";
  currentDirectory = root;
  initializeTransitions();
}

FileSystem::~FileSystem() = default;

std::shared_ptr<Node> FileSystem::getNode(const Path& path, bool parent) {
  auto pathChain = parent ? path.getParentChain() : path.getChain();
  auto currentNode = path.isAbsolute() ? root : currentDirectory;
  auto parentNode = pathChain.empty() ? currentNode : currentNode->findNode(pathChain, 0);

  if (parent && parentNode) {
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

  while (parentNode->getTarget()) {
    parentNode = parentNode->getTarget();
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
    gError = "Path not found";
    return false;
  }

  auto existingNode = parentNode->findNode(path.getFilename());
  if (existingNode) {
    if (!(existingNode->getType() == Node::DIRECTORY && existingNode->empty())) {
      gError = "File, link or not empty directory with such name already exists";
      return false;
    }
    return true;
  }

  if (parentNode->getType() != Node::DIRECTORY) {
    gError = "Target path is not a directory";
    return false;
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
    gError = "Path not found";
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

  if (parentNode->getType() != Node::DIRECTORY) {
    gError = "Target path is not a directory";
    return false;
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
    gError = "Source path not found";
    return false;
  }

  auto parentNodeTarget = getNode(target, false);
  if (!parentNodeTarget) {
    gError = "Destination path not found";
    return false;
  }

  auto sourceNode = parentNodeSource->findNode(source.getFilename());
  if (!sourceNode) {
    gError = "Source path not found";
    return false;
  }

  if (sourceNode->getType() == Node::LINK) {
    gError = "Link on link is not allowed";
    return false;
  }

  if (parentNodeTarget->getType() != Node::DIRECTORY) {
    gError = "Target path is not a directory";
    return false;
  }

  const Key& key = isDynamic ? "dlink" : "hlink";

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

  if (!parentNodeTarget->attachNode(key, newLink)) {
    log();
  }

  // assert(parentNodeTarget->attachNode(key, newLink));
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

  if (isPathContains(existingNode, currentDirectory)) {
    gError = "Can not remove directory you are currently working in";
    return false;
  }

  existingNode->clearFlags(existingNode);
  if (existingNode->isHardNode()) {
    gError = "Directory is referenced by a hard link";
    return false;
  }

  // deletes dynamic incoming links to this file system sub-ree
  existingNode->removeIncomingDynamicLinks();
  existingNode->removeOutgoingLinks();

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

  existingNode->clearFlags(existingNode);
  if (existingNode->isHardNode()) {
    gError = "File is referenced by a hard link";
    return false;
  }

  existingNode->removeIncomingDynamicLinks();
  existingNode->removeOutgoingLinks();

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
  }

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

  if (isPathContains(sourceNode, currentDirectory)) {
    gError = "Can not move directory you are currently working in";
    return false;
  }

  if (isPathContains(sourceNode, parentNodeTarget)) {
    gError = "Can not move node into itself";
    return false;
  }

  const Key& key = source.getFilename();

  if (parentNodeTarget->findNode(key)) {
    gError = "Node with such name already exists in the target directory";
    return false;
  }

  sourceNode->clearFlags(sourceNode);
  if (sourceNode->isHardNode()) {
    gError = "Node is referenced by a hard link";
    return false;
  }

  if (parentNodeTarget->getType() != Node::DIRECTORY) {
    gError = "Target path is not a directory";
    return false;
  }

  assert(parentNodeTarget->attachNode(key, sourceNode));
  assert(parentNodeSource->detachNode(key));

  sourceNode->mParent = parentNodeTarget;
  return true;
}

void FileSystem::log() const {
  std::stringstream ss;
  dump(ss);
  std::cout << ss.str();
}

const std::string& FileSystem::getLastError() {
  return gError;
}

bool FileSystem::isPathContains(const std::shared_ptr<Node>& node, const std::shared_ptr<Node>& current) {
  std::vector<std::shared_ptr<Node>> currentPath;
  std::vector<std::shared_ptr<Node>> path;

  root->getNodeStraightPath(current, currentPath);
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

ui64 FileSystem::size() const {
  return root->size();
}

std::ostream& FileSystem::dump(std::ostream &stream) const {
  std::vector<std::shared_ptr<Node>> currentPath;
  root->getNodeStraightPath(currentDirectory, currentPath);
  std::reverse(currentPath.begin(), currentPath.end());

  stream << "pwd [ ";
  for (auto it = currentPath.begin(); it != currentPath.end(); ++it) {
    stream << (*it)->mKey;
    if (std::distance(it, currentPath.end()) > 1) stream << "/";
  }
  stream << " ]\n";

  root->dump(stream);

  return stream;
}
