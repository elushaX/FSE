
#include "Directory.hpp"

#include <sstream>
#include <cassert>
#include <algorithm>

Directory::Directory() {
}

bool Directory::attachNode(const Key &newKey, std::shared_ptr<Node> newNode) {
  auto iterNode = mMembers.find(newKey);

  if (iterNode != mMembers.end()) {
    auto existingNode = iterNode->second;
    if (!(false && newNode->empty() && existingNode->empty())) {
      gError = "Such node already exists";
      return false;
    }
    return false; // exit silently
  }

  mMembers.insert({ newKey, newNode });
  return true;
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

  mMembers.erase(key);
  return true;
}

std::shared_ptr<Node> Directory::findNode(const Key& key) {
  auto iterNode = mMembers.find(key);
  return iterNode != mMembers.end() ? iterNode->second : nullptr;
}

std::shared_ptr<Node> Directory::findNode(const std::vector<Key>& path, ui32 currentDepth) {
  const Key& key = path[currentDepth];
  auto nextNode = mMembers.find(key);

  if (nextNode == mMembers.end()) {
    return nullptr;
  }

  if (path.size() == currentDepth + 1) {
    return nextNode->second;
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
  }
}

Directory::~Directory() {
  for (const auto& node : mMembers) {
    // delete node.second;
  }
}

std::shared_ptr<Node> Directory::clone() const {
  auto out = std::make_shared<Directory>(*this);
  for (auto& member : out->mMembers) {
    member.second->mParent = out;
  }
  return out;
}
