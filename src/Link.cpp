
#include "Link.hpp"

#include <sstream>
#include <cassert>
#include <algorithm>

Link::Link(Node* target, bool isHard) {
  mIsHard = isHard;
  mLink = target;

  if (mIsHard) {
    target->mIncomingHardLinks.push_back(this);
  } else {
    target->mIncomingDynamicLinks.push_back(this);
  }
}

Link::Link(const Link &node) : Node(node) {
  mLink = node.mLink;
  mIsHard = node.mIsHard;

  assert(mLink);
  if (mIsHard) {
    mLink->mIncomingHardLinks.push_back(this);
  } else {
    mLink->mIncomingDynamicLinks.push_back(this);
  }
}

Link *Link::clone() const {
  return new Link(*this);
}

Link::~Link() {
  // assert(mLink);
  // auto& links = mIsHard ? mLink->mIncomingHardLinks : mLink->mIncomingDynamicLinks;
  // links.erase(std::remove(links.begin(), links.end(), this), links.end());
  // Directory::updateTreeLinkCount(mLink);
  // mLink = nullptr;
}

Node *Link::getLink() const {
  return mLink;
}

bool Link::isHard() const {
  return mIsHard;
}

Node* Link::getTarget() {
return getLink();
}

Node* Link::findNode(const std::vector<Key>& path, ui32 currentDepth) {
  if (path.size() == currentDepth) {
    return this;
  }
  assert(mLink);
  return mLink->findNode(path, currentDepth + 1);
}


void Link::dumpUtil(std::stringstream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents) {
  indent(ss, currentDepth, indents);
  ss << key << (isHard() ?  " hlink[/" : " dlink[/");
  std::vector<const Node*> path;
  getNodeStraightPath(getLink(), path);
  std::reverse(path.begin(), path.end());
  for (auto tmp : path)
    ss << "X" << "/";
  ss << "]\n";
}

