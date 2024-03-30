
#include "Link.hpp"

#include <sstream>
#include <cassert>
#include <algorithm>

Link::Link() {}

Link::Link(const Link &node) : Node(node) {
  mLink = node.mLink;
  mIsHard = node.mIsHard;
}

std::shared_ptr<Node> Link::clone() const {
  return std::make_shared<Link>(*this);
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

void Link::removeOutgoingLinks() {
  auto target = mLink.lock();
  assert(target);

  auto& links = isHardLink() ? target->mIncomingHardLinks : target->mIncomingDynamicLinks;
  links.erase(std::remove_if(links.begin(), links.end(), [&](std::weak_ptr<Link>& node){
    auto targetLink = node.lock();
    return !targetLink || targetLink.get() == this;
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
  return getLink()->findNode(path, currentDepth + 1);
}

void Link::dumpUtil(std::stringstream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents) {
  indent(ss, currentDepth, indents);
  ss << key << (isHardLink() ?  " hlink[" : " dlink[");
  std::vector<std::shared_ptr<Node>> path;
  getNodeStraightPath(getLink(), path);
  std::reverse(path.begin(), path.end());

  for (auto it = path.begin(); it != path.end(); ++it) {
    ss << (*it)->mKey;
    if (std::distance(it, path.end()) > 1) ss << "/";
  }

  ss << "]\n";
}

