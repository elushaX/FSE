
#include "Node.hpp"
#include "Link.hpp"

#include <memory>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <iostream>

Node::Node(const Node &node) {
  mKey = node.mKey;
}

Node::~Node() {
  // assert(mIncomingHardLinks.empty());
  // for (auto dynamicLink : mIncomingDynamicLinks) {
  //  dynamicLink->mParent->detachNode(dynamicLink->mTreeNode->key.val);
  //  delete dynamicLink;
  //
}

std::shared_ptr<Node> Node::clone() const {
  return std::make_shared<Node>(*this);
}

ui32 Node::getMaxDepth() const {
  ui32 maxDepth = 1;
  getMaxDepthUtil(1, maxDepth);
  return maxDepth;
}

std::shared_ptr<Node> Node::findNode(const std::vector<Key>& path, ui32 currentDepth) {
  // assert(false);
  return nullptr;
}

void Node::clearFlags(std::shared_ptr<Node>& directory) {
  mWorkingNodeFlag = directory;
}

bool Node::isHardNode() const {
  return std::any_of(mIncomingHardLinks.begin(), mIncomingHardLinks.end(), [this](const std::weak_ptr<Link>& hardLink) {
    auto linkNode = hardLink.lock();
    auto ownLink = mWorkingNodeFlag.lock();
    assert(linkNode && ownLink);

    auto linkTarget = linkNode->mWorkingNodeFlag.lock();

    // if hard link in the same working tree it does not count
    return !linkTarget || ownLink != linkTarget;
  });
}

void Node::removeIncomingDynamicLinks() {
  for (auto& dynamicLink : mIncomingDynamicLinks) {
    auto linkNode = dynamicLink.lock();
    auto ownLink = mWorkingNodeFlag.lock();
    assert(linkNode && ownLink);

    auto linkTarget = linkNode->mWorkingNodeFlag.lock();
    if (linkTarget && ownLink == linkTarget) {
      // continue;
    }

    auto parentNode = linkNode->mParent.lock();
    assert(parentNode);
    parentNode->detachNode(linkNode->mKey);
  }
}

std::shared_ptr<Node> Node::getTarget() {
  return nullptr;
}

void Node::getNodeStraightPath(const std::shared_ptr<Node>& node, std::vector<std::shared_ptr<Node>>& path) const {
  if (!node) return;
  path.push_back(node);
  getNodeStraightPath(node->mParent.lock(), path);
}

void Node::dump(std::stringstream& ss) {
  std::vector<bool> indents;
  indents.resize(getMaxDepth());
  dumpUtil(ss, mKey, 0, indents);
  ss << "\n";
}

void Node::dumpUtil(std::stringstream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents) {
  indent(ss, currentDepth, indents);
  ss << key;
  ss << " [h" << mIncomingHardLinks.size() << ": d" << mIncomingDynamicLinks.size() << "] ";
  ss << "\n";
}

void Node::indent(std::stringstream & ss, ui32 depth, std::vector<bool>& indents) {
  if (!depth) return;
  for (auto i  = 0; i < depth - 1; i++) {
    ss << (indents[i] ? " |" : "  ");
  }
  ss << " |_";
}