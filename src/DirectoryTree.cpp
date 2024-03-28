
#include "DirectoryTree.hpp"

#include <sstream>

std::string gError;
bool gDebug = true;

Node::~Node() = default;

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
  assert(mLink);
  auto& links = mIsHard ? mLink->mIncomingHardLinks : mLink->mIncomingDynamicLinks;
  links.erase(std::remove(links.begin(), links.end(), this), links.end());
  Directory::updateTreeLinkCount(mLink);
  mLink = nullptr;
}

Directory::Directory() {
  mType = DIRECTORY;
}

Directory::~Directory() {
  mMembers.traverseInorder(mMembers.getRoot(), [](DirectoryTree::Node* node){
    delete node->data;
  });
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
  DirectoryTree::Node* iterNode = mMembers.find(DirectoryKey(newKey));
  if (iterNode) {
    if (iterNode->data->mType == newNode->mType) return false; // exit silently
    gError = "Can not add node";
    return false;
  }

  mMembers.insert(DirectoryKey(newKey), newNode);

  newNode->mParent = this;
  updateTreeLinkCount(this);
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
  DirectoryTree::Node* removeNode = mMembers.find(DirectoryKey(key));
  if (!removeNode) {
    gError = "Invalid path";
    return false;
  }

  if (removeNode->key.incomingLinksHard || removeNode->key.incomingLinksDynamic) {
    gError = "Cannot modify node with incoming hard links";
    return false;
  }

  removeNode->mParent = nullptr;
  mMembers.remove(DirectoryKey(key));

  updateTreeLinkCount(this);
  return true;
}

Node* Directory::findNode(const Key& key) {
  DirectoryTree::Node* iterNode = mMembers.find(DirectoryKey(key));
  return iterNode ? iterNode->data : nullptr;
}

Node* Directory::findNode(const std::vector<Key>& path, ui32 currentDepth) {
  if (path.size() == currentDepth) {
    return this;
  }

  const Key& key = path[currentDepth];
  DirectoryTree::Node* iterNode = mMembers.find(DirectoryKey(key));

  if (!iterNode) {
    return nullptr;
  }

  Node* node = iterNode->data;

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

void Directory::updateTreeLinkCount(Node* node) {
  if (!node || !node->mTreeNode) return;

  node->mTreeNode->key.updateTreeCacheCallBack(*node->mTreeNode);

  if (node->mTreeNode->mParent) {
    updateTreeLinkCount(node->mTreeNode->mParent->data);
  } else {
    updateTreeLinkCount(node->mParent);
  }
}

void Directory::getMaxDepthUtil(ui32 depth, ui32& maxDepth) const {
  if (!mMembers.getRoot()) return;
  maxDepth = std::max(depth, maxDepth);
  mMembers.traverseInorder(mMembers.getRoot(), [&](const DirectoryTree::Node* node){
    if (node->data->mType == DIRECTORY) {
      ((Directory*)node->data)->getMaxDepthUtil(++depth, maxDepth);
    }
  });
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
  indents[currentDepth] = true;
  currentDepth++;

  auto lastNode = mMembers.maxNode(mMembers.getRoot());
  traverseInorder([&](const DirectoryTree::Node* node){
    if (lastNode == node) indents[currentDepth - 1] = false;

    indent(ss, currentDepth, indents);

    switch (node->data->mType) {
      case Node::DIRECTORY:
        ss << node->key.val;
        if (gDebug)  ss << " [" << node->key.incomingLinksHard << ":" << node->key.incomingLinksDynamic << "]";
        ss << "\n";
        ((Directory*) node->data)->dumpUtil(ss, currentDepth, indents);
        return;

      case Node::LINK: {
        auto linkNode = ((Link*)node->data);
        ss << node->key.val << (linkNode->isHard() ?  " hlink[/" : " dlink[/");
        std::vector<const Key*> path;
        getNodePath(linkNode->getLink(), path);
        std::reverse(path.begin(), path.end());
        for (auto key : path) ss << *key << "/";
        ss << "]\n";
        return;
      }

      case Node::FILE:
        ss << node->key.val;
        if (gDebug) ss << " [file]";
        ss << "\n";
        return;

      default:
        ss << " ERROR \n";
        return;
    }
  });
}

void Directory::getNodePath(Node* node, std::vector<const Key*>& path) const {
  if (!node || !node->mTreeNode) return;
  path.push_back(&node->mTreeNode->key.val);
  getNodePath(node->mParent, path);
}

ui64 Directory::size() const {
  return mMembers.size();
}

Directory::Directory(const Directory &node) : Node(node) {
  node.traverseInorder([&](const DirectoryTree::Node* node){
    auto newNode = node->data->clone();
    mMembers.insert(node->key, newNode);
    newNode->mParent = this;
    if (newNode->mType == LINK) {
      updateTreeLinkCount(((Link*)newNode)->getLink());
    }
  });

  updateTreeLinkCount(this);
}

Directory *Directory::clone() const {
  return new Directory(*this);
}
