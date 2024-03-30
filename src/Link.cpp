
#include "Link.hpp"

#include <sstream>
#include <cassert>
#include <algorithm>
#include <iostream>

Link::Link() = default;

Link::Link(const Link &node) : Node(node) {}

std::shared_ptr<Node> Link::clone() const {
  auto out = std::make_shared<Link>(*this);
  Link::linkNodes(out, getLink(), isHardLink());
  return out;
}

Link::~Link() = default;

bool Link::linkNodes(const std::shared_ptr<Link>& link, const std::shared_ptr<Node>& target, bool hard) {
  if (target->getType() == LINK) return false;

  // assert(!link->mLink.lock());

  link->mIsHard = hard;
  link->mLink = target;

  if (hard) {
    target->mIncomingHardLinks.push_back(link);
  } else {
    target->mIncomingDynamicLinks.push_back(link);
  }

  return true;
}

// TODO : move from link to node class or vise versa
void Link::removeOutgoingLinks() {
  auto target = mLink.lock();
  assert(target);

  auto& links = isHardLink() ? target->mIncomingHardLinks : target->mIncomingDynamicLinks;
  links.erase(std::remove_if(links.begin(), links.end(), [&](std::weak_ptr<Link>& node){
    auto targetLink = node.lock();
    assert(targetLink);
    return targetLink.get() == this;
  }), links.end());
}

std::shared_ptr<Node> Link::getLink() const {
  auto target = mLink.lock();
  assert(target);
  return target;
}

bool Link::isHardLink() const {
  return mIsHard;
}

std::shared_ptr<Node> Link::getTarget() {
  return getLink();
}

std::shared_ptr<Node> Link::findNode(const std::vector<Key>& path, ui32 currentDepth) {
  assert(path.size() != currentDepth);
  return getLink()->findNode(path, currentDepth);
}

void Link::dumpUtil(std::stringstream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents) {
  std::vector<std::shared_ptr<Node>> path;
  getNodeStraightPath(getLink(), path);
  std::reverse(path.begin(), path.end());

  indent(ss, currentDepth, indents);

  ss << key;
  ss << (isHardLink() ?  " hlink[" : " dlink[");
  // ss << " [h" << mIncomingHardLinks.size() << ": d" << mIncomingDynamicLinks.size() << "] ";

  for (auto it = path.begin(); it != path.end(); ++it) {
    ss << (*it)->mKey;
    if (std::distance(it, path.end()) > 1) ss << "/";
  }

  ss << "]\n";
}

