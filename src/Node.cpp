
#include "Node.hpp"

#include <memory>
#include <sstream>
#include <cassert>
#include <algorithm>

std::string gError;
bool gDebug = true;

Node::Node(const Node &node) {}

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
  assert(false);
  return nullptr;
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
  dumpUtil(ss, "/", 0, indents);
  ss << "\n";
}

void Node::dumpUtil(std::stringstream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents) {
  indent(ss, currentDepth, indents);
  ss << key;
  if (gDebug) ss << " [file]";
  ss << "\n";
}

void Node::indent(std::stringstream & ss, ui32 depth, std::vector<bool>& indents) {
  if (!depth) return;
  for (auto i  = 0; i < depth - 1; i++) {
    ss << (indents[i] ? " |" : "  ");
  }
  ss << " |_ ";
}