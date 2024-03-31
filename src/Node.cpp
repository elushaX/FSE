
#include "Node.hpp"
#include "Link.hpp"

#include <memory>
#include <cassert>
#include <algorithm>
#include <iostream>

Node::Node(const Node &node) {
  mKey = node.mKey;
}

Node::~Node() = default;

std::shared_ptr<Node> Node::clone() const {
  return std::make_shared<Node>(*this);
}

ui32 Node::getMaxDepth() const {
  ui32 maxDepth = 1;
  getMaxDepthUtil(1, maxDepth);
  return maxDepth;
}

std::shared_ptr<Node> Node::findNode(const std::vector<Key>& path, ui32 currentDepth) {
  return nullptr;
}

void Node::clearFlags(std::shared_ptr<Node>& directory) {
  mWorkingNodeFlag = directory;
}

bool Node::isHardNode() const {
  return std::any_of(mIncomingLinks.begin(), mIncomingLinks.end(), [this](const std::weak_ptr<Link>& hardLink) {
    auto linkNode = hardLink.lock();

    if (!linkNode->isHardLink()) return false;

    auto ownLink = mWorkingNodeFlag.lock();
    assert(linkNode && ownLink);

    auto linkTarget = linkNode->mWorkingNodeFlag.lock();

    // if hard link in the same working tree it does not count
    return !linkTarget || ownLink != linkTarget;
  });
}

void Node::removeIncomingDynamicLinks() {
  for (auto& dynamicLink : mIncomingLinks) {
    auto linkNode = dynamicLink.lock();

    if (linkNode->isHardLink()) continue;

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

std::ostream& Node::dump(std::ostream &stream) {
  std::vector<bool> indents;
  indents.resize(getMaxDepth());
  dumpUtil(stream, mKey, 0, indents);
  stream << "\n";
  return stream;
}

void Node::dumpUtil(std::ostream &stream, const Key& key, ui32 currentDepth, std::vector<bool>& indents) {
  indent(stream, currentDepth, indents);
  stream << key;
  // ss << " [h" << mIncomingHardLinks.size() << ": d" << mIncomingDynamicLinks.size() << "] ";
  stream << "\n";
}

void Node::indent(std::ostream& ss, ui32 depth, std::vector<bool>& indents) {
  if (!depth) return;
  for (auto i  = 0; i < depth - 1; i++) {
    ss << (indents[i] ? " |" : "  ");
  }
  ss << " |_";
}

void Node::removeOutgoingLinks() {}

bool Node::detachNode(const Key &key) {
  return false;
}

bool Node::attachNode(const Key &newKey, const std::shared_ptr<Node> &newNode) {
  return false;
}

std::shared_ptr<Node> Node::findNode(const Key &path) {
  return nullptr;
}

bool Node::empty() const {
  return !size();
}

void Node::getMaxDepthUtil(ui32 depth, ui32 &maxDepth) const {}

Node::NodeType Node::getType() const {
  return Node::FILE;
}

ui64 Node::size() const {
  return 0;
}
