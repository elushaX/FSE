
#include "Directory.hpp"

#include <sstream>
#include <cassert>
#include <algorithm>

Directory::Directory() {
}

bool Directory::attachNode(const std::vector<Key>& directoryPath, const Key& newKey, Node* newNode) {
  Node* node = findNode(directoryPath);

  if (!node) {
    gError = "Invalid path";
    return false;
  }

  while (node->getTarget() != node) node = node->getTarget();
  return node->attachNode(newKey, newNode);
}

bool Directory::attachNode(const Key &newKey, Node *newNode) {
  auto iterNode = mMembers.find(newKey);

  if (iterNode != mMembers.end()) {
    Node* existingNode = iterNode->second;
    if (!(typeid(*existingNode) == typeid(*newNode) && newNode->empty() && existingNode->empty())) {
      gError = "Such node already exists";
      return false;
    }
    return false; // exit silently
  }

  mMembers.insert({ newKey, newNode });
  newNode->mParent = this;
  return true;
}

bool Directory::detachNode(const std::vector<Key>& directoryPath, const Key& key) {
  Node* node = findNode(directoryPath);
  return node->detachNode(key);
}

bool Directory::detachNode(const Key& key) {
  auto removeNode = mMembers.find(key);
  if (removeNode == mMembers.end()) {
    gError = "Invalid path";
    return false;
  }

  //if (removeNode->key.incomingLinksHard || removeNode->key.incomingLinksDynamic) {
  //  gError = "Cannot modify node with incoming hard links";
  //  return false;
  //}

  removeNode->second->mParent = nullptr;
  mMembers.erase(key);
  return true;
}

Node* Directory::findNode(const Key& key) {
  auto iterNode = mMembers.find(key);
  return iterNode != mMembers.end() ? iterNode->second : nullptr;
}

Node* Directory::findNode(const std::vector<Key>& path, ui32 currentDepth) {
  if (path.size() == currentDepth) {
    return this;
  }

  const Key& key = path[currentDepth];
  auto nextNode = mMembers.find(key);

  if (nextNode == mMembers.end()) {
    return nullptr;
  }

  return nextNode->second->findNode(path, currentDepth + 1);
}

void Directory::getMaxDepthUtil(ui32 depth, ui32& maxDepth) const {
  if (mMembers.empty()) return;
  maxDepth = std::max(depth, maxDepth);
  for (auto& node : mMembers) {
    node.second->getMaxDepthUtil(++depth, maxDepth);
  }
}

void Directory::dumpUtil(std::stringstream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents) {

  indents[currentDepth] = true;

  indent(ss, currentDepth, indents);
  ss << key;
  ss << " [" << mIncomingHardLinks.size() << ":" << mIncomingDynamicLinks.size() << "]";
  ss << "\n";

  currentDepth++;

  if (mMembers.empty()) return;

  const auto& lastNode = mMembers.rbegin()->first;
  for (auto & member : mMembers) {
    if (lastNode == member.first) indents[currentDepth - 1] = false;
    member.second->dumpUtil(ss, member.first, currentDepth, indents);
  }
}

ui64 Directory::size() const {
  return mMembers.size();
}

Directory::Directory(const Directory &node) : Node(node) {
  for (auto & member : node.mMembers) {
    auto newNode = member.second->clone();
    mMembers.insert({ member.first, newNode });
    newNode->mParent = this;
  }
}

Directory::~Directory() {
  for (const auto& node : mMembers) {
    delete node.second;
  }
}

Directory *Directory::clone() const {
  return new Directory(*this);
}
