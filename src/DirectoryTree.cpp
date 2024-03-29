
#include "DirectoryTree.hpp"

#include <sstream>
#include <cassert>
#include <algorithm>

std::string gError;
bool gDebug = true;

Node::~Node() {
  // assert(mIncomingHardLinks.empty());
  // for (auto dynamicLink : mIncomingDynamicLinks) {
  //  dynamicLink->mParent->detachNode(dynamicLink->mTreeNode->key.val);
  //  delete dynamicLink;
  //
}

Node::Node(const Node &node) {
  mType = node.mType;
}

Node *Node::clone() const {
  return new Node(*this);
}

File::File() {
  mType = Type::FILE;
}

File::File(const File& node) : Node(node) {
  // nothing to do
}

File *File::clone() const {
  return new File(*this);
}

Link::Link(Node* target, bool isHard) {
  mType = LINK;
  mIsHard = isHard;
  mLink = target;

  if (mIsHard) {
    target->mIncomingHardLinks.push_back(this);
  } else {
    target->mIncomingDynamicLinks.push_back(this);
  }
}

Node *Link::getLink() const {
  return mLink;
}

bool Link::isHard() const {
  return mIsHard;
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

Directory::Directory() {
  mType = DIRECTORY;
}

bool Directory::attachNode(const std::vector<Key>& directoryPath, const Key& newKey, Node* newNode) {
  Node* node = findNode(directoryPath, 0);

  while (node && node->mType == LINK) node = ((Link*)node)->getLink();

  if (!node || node->mType != DIRECTORY) {
    gError = "Invalid path";
    return false;
  }

  return ((Directory*) node)->attachNode(newKey, newNode);
}

bool Directory::attachNode(const Key &newKey, Node *newNode) {
  auto iterNode = mMembers.find(newKey);
  if (iterNode != mMembers.end()) {
    if (iterNode->second->mType == newNode->mType) return false; // exit silently
    gError = "Can not add node";
    return false;
  }

  mMembers.insert({ newKey, newNode });

  newNode->mParent = this;
  return true;
}

bool Directory::detachNode(const std::vector<Key>& directoryPath, const Key& key) {
  Node* node = findNode(directoryPath, 0);
  if (!node || node->mType != DIRECTORY) {
    gError = "Invalid path";
    return false;
  }

  return ((Directory*) node)->detachNode(key);
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
  auto iterNode = mMembers.find(key);

  if (iterNode == mMembers.end()) {
    return nullptr;
  }

  Node* node = iterNode->second;

  // link on link is not allowed
  while (true) {
    switch (node->mType) {
      case Node::FILE:
        if (currentDepth == path.size() - 1) return node;
        return nullptr;

      case Node::DIRECTORY:
        return ((Directory*)node)->findNode(path, ++currentDepth);

      case Node::LINK:
        if (currentDepth == path.size() - 1) return node;
        node = ((Link*)node)->getLink();
        break;

      default:
        return nullptr;
    }
  }
}

void Directory::getMaxDepthUtil(ui32 depth, ui32& maxDepth) const {
  if (mMembers.empty()) return;
  maxDepth = std::max(depth, maxDepth);
  for (auto& node : mMembers) {
    if (node.second->mType == DIRECTORY) {
      ((Directory*)node.second)->getMaxDepthUtil(++depth, maxDepth);
    }
  }
}

ui32 Directory::getMaxDepth() const {
  ui32 maxDepth = 1;
  getMaxDepthUtil(1, maxDepth);
  return maxDepth;
}

void Directory::dump(std::stringstream& ss) {
  std::vector<bool> indents;
  indents.resize(getMaxDepth());
  dumpUtil(ss, 0, indents);
  ss << "\n";
}

static void indent(std::stringstream & ss, ui32 depth, std::vector<bool>& indents) {
  if (!depth) return;
  for (auto i  = 0; i < depth - 1; i++) {
    ss << (indents[i] ? " |" : "  ");
  }
  ss << " |_ ";
}

void Directory::dumpUtil(std::stringstream& ss, ui32 currentDepth, std::vector<bool>& indents) {
  if (mMembers.empty())
    return;

  indents[currentDepth] = true;
  currentDepth++;

  const auto& lastNode = mMembers.rbegin()->first;

  for (auto & member : mMembers) {
    if (lastNode == member.first)
      indents[currentDepth - 1] = false;

    indent(ss, currentDepth, indents);

    switch (member.second->mType) {
      case Node::DIRECTORY:
        ss << member.first;
        if (gDebug)  ss << " [" << member.second->mIncomingHardLinks.size() << ":" << member.second->mIncomingDynamicLinks.size() << "]";
        ss << "\n";
        ((Directory*) member.second)->dumpUtil(ss, currentDepth, indents);
        break;

      case Node::LINK: {
        auto linkNode = ((Link*)member.second);
        ss << member.first << (linkNode->isHard() ?  " hlink[/" : " dlink[/");
        std::vector<const Node*> path;
        getNodeStraightPath(linkNode->getLink(), path);
        std::reverse(path.begin(), path.end());
        for (auto key : path)
          ss << "X" << "/";
        ss << "]\n";
        break;
      }

      case Node::FILE:
        ss << member.first;
        if (gDebug) ss << " [file]";
        ss << "\n";
        break;

      default:
        ss << " ERROR \n";
        break;
    }
  }
}

void Directory::getNodeStraightPath(Node* node, std::vector<const Node*>& path) const {
  if (!node) return;
  path.push_back(node);
  getNodeStraightPath(node->mParent, path);
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
  for (auto node : mMembers) {
    delete node.second;
  }
}

Directory *Directory::clone() const {
  return new Directory(*this);
}
