
#include "DirectoryTree.hpp"

#include <sstream>

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

Directory::~Directory() {
  mMembers.traverseInorder(mMembers.getRoot(), [](DirectoryTree::Node* node){
    delete node->data;
  });
}

bool Directory::attachNode(const std::vector<Key>& directoryPath, const Key& newKey, Node* newNode) {
  Node* node = findNode(directoryPath, 0);
  if (!node || node->mType != DIRECTORY) {
    gError = "Invalid path";
    return false;
  }

  auto directory = ((Directory*) node);

  DirectoryTree::Node* iterNode = directory->mMembers.find(DirectoryKey(newKey));
  if (iterNode) {
    if (iterNode->data->mType == newNode->mType) return false; // exit silently
    gError = "Cant add node";
    return false;
  }

  directory->mMembers.insert(DirectoryKey(newKey), newNode);

  newNode->mParent = directory;
  updateTreeLinkCount(directory);
  return true;
}

bool Directory::detachNode(const std::vector<Key>& directoryPath, const Key& key) {
  Node* node = findNode(directoryPath, 0);
  if (!node || node->mType != DIRECTORY) {
    gError = "Invalid path";
    return false;
  }

  auto directory = ((Directory*) node);

  DirectoryTree::Node* removeNode = directory->mMembers.find(DirectoryKey(key));
  if (!removeNode) {
    gError = "Invalid path";
    return false;
  }

  directory->mMembers.remove(DirectoryKey(key));
  removeNode->mParent = nullptr;

  updateTreeLinkCount(directory);
  return true;
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
  ss << " |_";
}

void Directory::dumpUtil(std::stringstream& ss, ui32 currentDepth, std::vector<bool>& indents) {
  indents[currentDepth] = true;
  currentDepth++;

  auto lastNode = mMembers.maxNode(mMembers.getRoot());
  traverseInorder([&](const DirectoryTree::Node* node){
    if (lastNode == node) indents[currentDepth - 1] = false;

    indent(ss, currentDepth, indents);
    ss << node->key.val << "\n";

    switch (node->data->mType) {
      case Node::DIRECTORY:
        ((Directory*) node->data)->dumpUtil(ss, currentDepth, indents);
        return;

      default:
        // do nothing for now
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