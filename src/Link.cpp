
#include "Link.hpp"

#include <sstream>
#include <cassert>
#include <algorithm>

Link::Link() {}

Link::Link(const Link &node) : Node(node) {}

std::shared_ptr<Node> Link::clone() const {
  return std::make_shared<Link>(*this);
}

Link::~Link() = default;

bool Link::linkNodes(const std::shared_ptr<Link>& link, const std::shared_ptr<Node>& target, bool hard) {
  if (target->getType() == LINK) return false;

  assert(!link->mLink.lock());

  link->mIsHard = hard;
  link->mLink = target;

  if (hard) {
    target->mIncomingHardLinks.push_back(link);
  } else {
    target->mIncomingDynamicLinks.push_back(link);
  }

  return true;
}

std::shared_ptr<Node> Link::getLink() const {
  auto target = mLink.lock();
  assert(target);
  return target;
}

bool Link::isHard() const {
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
  ss << key << (isHard() ?  " hlink[" : " dlink[");
  std::vector<std::shared_ptr<Node>> path;
  getNodeStraightPath(getLink(), path);
  std::reverse(path.begin(), path.end());
  for (auto& node : path)
    ss << node->mKey << "/";
  ss << "]\n";
}

