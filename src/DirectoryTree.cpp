
#include "DirectoryTree.hpp"

std::string gError;

Node::~Node() = default;

File::File() {
  mType = Type::FILE;
}

Link::Link() {
  mType = LINK;
}

Node *Link::getLink() const {
  return mLink;
}

Directory::Directory() {
  mType = DIRECTORY;
}

Directory::~Directory() = default;

bool Directory::attachNode(const std::vector<Key>& directoryPath, const Key& newKey, Node* newNode) {
  Node* node = findNode(directoryPath, 0);
  if (!node) {
    gError = "Invalid path";
    return false;
  }

  if (node->mType != DIRECTORY) {
    gError = "given path is not a directory";
    return false;
  }

  auto directory = ((Directory*) node);

  Node* existingNode = directory->mMembers.find(DirectoryKey(newKey))->data;
  if (existingNode) {
    if (existingNode->mType == newNode->mType) return false; // exit silently
    gError = "Cant add node";
    return false;
  }

  directory->mMembers.insert(DirectoryKey(newKey), newNode);
  updateTreeLinkCount(newNode);

  return true;
}

Node* Directory::findNode(const std::vector<Key>& path, ui32 currentDepth) {
  if (path.size() == currentDepth) {
    return this;
  }

  const Key& key = path[currentDepth];
  Node* node = mMembers.find(DirectoryKey(key))->data;

  if (!node) {
    return nullptr;
  }

  // link on link is not allowed
  while (true) {
    switch (node->mType) {
      case Node::FILE:
        if (currentDepth == path.size() - 1) return node;
        return nullptr;

      case Node::DIRECTORY:
        return ((Directory*)node)->findNode(path, ++currentDepth);

      case Node::LINK:
        node = ((Link*)node)->getLink();
        break;

      default:
        return nullptr;
    }
  }
}

void Directory::updateTreeLinkCount(Node* node) {
  // TODO : update all caches all the way up to '/'
}

void Directory::detachNode(Node* node) {
  // mMembers.remove();
}

void Directory::getMaxDepthUtil(ui32 depth, ui32& maxDepth) const {
  if (!mMembers.getRoot()) return;
  maxDepth = std::max(depth, maxDepth);
  mMembers.traverseInorder(mMembers.getRoot(), [&](const DirectoryTree::Node* node){
    if (node->data->mType == DIRECTORY) {
      ((Directory*)node)->getMaxDepthUtil(++depth, maxDepth);
    }
  });
}

ui32 Directory::getMaxDepth() const {
  ui32 maxDepth = 1;
  getMaxDepthUtil(1, maxDepth);
  return maxDepth;
}