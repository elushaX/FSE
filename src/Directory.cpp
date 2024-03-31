
#include "Directory.hpp"
#include "Link.hpp"

#include <sstream>
#include <algorithm>
#include <iostream>

Directory::Directory() = default;

Directory::~Directory() {
}

bool Directory::attachNode(const Key &newKey, std::shared_ptr<Node> newNode) {
  assert(mMembers.find(newKey) == mMembers.end());
  mMembers.insert({ newKey, newNode });
  newNode->mKey = newKey;
  return true;
}

bool Directory::detachNode(const Key& key) {
  assert(mMembers.find(key) != mMembers.end());
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

void Directory::dumpUtil(std::ostream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents) {
  indents[currentDepth] = true;
  indent(ss, currentDepth, indents);

  ss << key;
  // ss << " [h" << mIncomingHardLinks.size() << ": d" << mIncomingDynamicLinks.size() << "] ";
  // ss << " [" << mWorkingNodeFlag.lock().get() << "] ";
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

std::shared_ptr<Node> Directory::clone() const {
  auto out = std::make_shared<Directory>(*this);
  for (auto& member : out->mMembers) {
    member.second->mParent = out;
  }

  return out;
}

void Directory::clearFlags(std::shared_ptr<Node> &directory) {
  traverse(directory, [&](std::shared_ptr<Node>& node) {
    node->mWorkingNodeFlag = directory;
  });
}

bool Directory::isHardNode() const {
  if (Node::isHardNode()) return true;
  return std::any_of(mMembers.begin(), mMembers.end(), [](const auto& member) {
    return member.second->isHardNode();
  });
}

void Directory::removeIncomingDynamicLinks() {
  Node::removeIncomingDynamicLinks();
  for (auto& member : mMembers) {
    member.second->removeIncomingDynamicLinks();
  }
}

void Directory::removeOutgoingLinks() {
  for (auto& member : mMembers) {
    member.second->removeOutgoingLinks();
  }
}