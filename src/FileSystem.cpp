#include "FileSystem.hpp"
#include <iostream>
#include <cassert>

static std::string gError;

void Node::updateTreeCache() {
  // TODO update cache
}

File::File() {
  type = Type::FILE;
}

Link::Link() {
  type = LINK;
}

Directory::Directory() {
  type = DIRECTORY;
}

bool Directory::attachNode(const std::vector<Key>& directoryPath, const Key& newKey, Node* newNode) {
  Node* node = findNode(directoryPath, 0);
  if (!node) {
    gError = "Invalid path";
    return false;
  }

  if (node->type != DIRECTORY) {
    gError = "given path is not a directory";
    return false;
  }

  auto directory = ((Directory*) node);

  Node* existingNode = directory->treeSearch(newKey);
  if (existingNode) {
    if (existingNode->type == newNode->type) return false; // exit silently
    gError = "Cant add node";
    return false;
  }

  directory->treeInsert(newKey, newNode);
  updateTreeLinkCount(newNode);

  return true;
}

Node* Directory::findNode(const std::vector<Key>& path, ui32 currentDepth) {
  if (path.size() == currentDepth) {
    return this;
  }

  const Key& key = path[currentDepth];
  Node* node = treeSearch(key);

  if (!node) {
    return nullptr;
  }

  // link on link is not allowed
  while (true) {
    switch (node->type) {
      case Node::FILE:
        if (currentDepth == path.size() - 1) return node;
        return nullptr;

      case Node::DIRECTORY:
        return ((Directory*)node)->findNode(path, ++currentDepth);

      case Node::LINK:
        node = ((Link*)node)->link;
        break;

      default:
        return nullptr;
    }
  }
}

Node* Directory::treeSearch(const Key& key) {
  if (!members) return nullptr;
  Node* iterator = members;
  while (iterator) {
    if (key > iterator->key) {
      iterator = iterator->right;
    } else if (key < iterator->key) {
      iterator = iterator->left;
    } else {
      return iterator;
    }
  }
  return nullptr;
}

void Directory::updateTreeLinkCount(Node* node) {
  // TODO : update all caches all the way up to '/'
}

void Directory::detachNode(Node* node) {
  // TODO : remove util from avl tree
  // TODO : update all cache all the way up to the root (due to the links)
  // TODO : dont relocate nodes (due existing links to the nodes), only change tree pointers
}

// TODO : user avl tree insertion
// TODO : check for existing file
// TODO : return true if successful (node inserted)
// TODO : dont relocate nodes (due existing links to the nodes), only change tree pointers
void Directory::treeInsert(const Key& newKey, Node* newNode) {
  newNode->key = newKey;
  members = insertUtil(members, newKey, newNode);
}

// recursively returns valid isLeft or isRight child or root
Node* Directory::insertUtil(Node* head, const Key& key, Node* aNode) {

  Node* insertedNode;

  if (head == nullptr) {
    size++;
    aNode->updateTreeCache();
    return aNode;
  } else if (head->key == key) {
    return head;
  } else if (key > head->key) {
    insertedNode = insertUtil(head->right, key, aNode);
    head->right = insertedNode;
    insertedNode->parent = head;
  } else {
    insertedNode = insertUtil(head->left, key, aNode);
    head->left = insertedNode;
    insertedNode->parent = head;
  }

  // update height
  head->height = 1 + std::max(getNodeHeight(head->right), getNodeHeight(head->left));

  int balance = int(getNodeHeight(head->right) - getNodeHeight(head->left));

  if (balance > 1) {
    if (key > head->right->key) {
      return rotateLeft(head);
    } else {
      head->right = rotateRight(head->right);
      return rotateLeft(head);
    }
  } else if (balance < -1) {
    if (key < head->left->key) {
      return rotateRight(head);
    } else {
      head->left = rotateLeft(head->left);
      return rotateRight(head);
    }
  }

  head->updateTreeCache();

  return head;
}

// returns new head
Node* Directory::rotateLeft(Node* pivot) {
  Node* const head = pivot;
  Node* const right = pivot->right;
  Node* const right_left = right->left;
  Node* const parent = pivot->parent;

  // parents
  if (right_left) right_left->parent = head;
  head->parent = right;
  right->parent = parent;

  // children
  head->right = right_left;
  right->left = head;

  // heights
  head->height = 1 + std::max(getNodeHeight(head->left), getNodeHeight(head->right));
  right->height = 1 + std::max(getNodeHeight(right->left), getNodeHeight(right->right));

  // cache
  head->updateTreeCache();
  right->updateTreeCache();

  return right;
}

Node* Directory::rotateRight(Node* pivot) {
  Node* const head = pivot;
  Node* const left = pivot->left;
  Node* const left_right = left->right;
  Node* const parent = pivot->parent;

  // parents
  if (left_right) left_right->parent = head;
  head->parent = left;
  left->parent = parent;

  // children
  head->left = left_right;
  left->right = head;

  // heights
  head->height = 1 + std::max(getNodeHeight(head->left), getNodeHeight(head->right));
  left->height = 1 + std::max(getNodeHeight(left->left), getNodeHeight(left->right));

  // cache
  head->updateTreeCache();
  left->updateTreeCache();

  return left;
}

ui32 Directory::getNodeHeight(const Node* node) { return node ? node->height : -1; }

Node* Directory::maxNode() const {
  Node* head = members;
  if (!head) return nullptr;
  while (head->right != nullptr) {
    head = head->right;
  }
  return head;
}

void Directory::getMaxDepthUtil(ui32 depth, ui32& maxDepth) const {
  if (!members) return;
  maxDepth = std::max(depth, maxDepth);
  traverseInorderUtil(members, [&](Node* node){
    if (node->type == DIRECTORY) {
      ((Directory*)node)->getMaxDepthUtil(++depth, maxDepth);
    }
  });
}

ui32 Directory::getMaxDepth() const {
  ui32 maxDepth = 1;
  getMaxDepthUtil(1, maxDepth);
  return maxDepth;
}


FileSystem::FileSystem() {
    root = new Directory();
    currentDirectory = root;
    root->key = "/";
    initializeTransitions();
  }

FileSystem::~FileSystem() {
    delete root;
  }

bool FileSystem::makeDirectory(const Path& path) {
  if (path.getDepth() < 1 || path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  Directory* parentDirectory = path.isAbsolute() ? root : currentDirectory;

  auto existingNode = parentDirectory->findNode(path.getChain());
  if (existingNode) {
    if (existingNode->type == Node::DIRECTORY) return true;
    gError = "Cant create directory, such node exists";
    return false;
  }

  auto newDirectory = new Directory();
  assert(parentDirectory->attachNode(path.getParentChain(), path.getFilename(), newDirectory));

  return true;
}

bool FileSystem::changeCurrent(const Path& path) {
  if (path.isInvalid()) {
    gError = "Invalid path";
    return false;
  }

  auto node = path.isAbsolute() ? root->findNode(path.getChain(), 0) : currentDirectory->findNode(path.getChain(), 0);
  if (!node || node->type != Node::DIRECTORY) {
    gError = "No such directory";
    return false;
  }

  currentDirectory = (Directory*) node;
  return true;
}

void FileSystem::log() const {
  std::stringstream ss;
  std::vector<bool> indents;
  indents.resize(root->getMaxDepth());
  logNode(ss, root, 0, indents);
  std::cout << ss.str() << "\n";
}


void FileSystem::logNode(std::stringstream& ss, const Node* node, int depth, std::vector<bool>& indents) const {
  switch (node->type) {
    case Node::DIRECTORY:
      return logDirectory(ss, (Directory*) node, depth, indents);
    case Node::FILE:
      return logFile(ss, (File*) node, depth, indents);
    case Node::LINK:
      return logLink(ss, (Link*) node, depth, indents);
  }
}

void FileSystem::indent(std::stringstream & ss, int depth, std::vector<bool>& indents) const {
  if (!depth) return;
  for (auto i  = 0; i < depth - 1; i++) {
    ss << (indents[i] ? " |" : "  ");
  }
  ss << " |_";
}

void FileSystem::logDirectory(std::stringstream & ss, const Directory* node, int depth, std::vector<bool>& indents) const {
  ss << (node == currentDirectory ? "* " : "  ");
  indent(ss, depth, indents);
  ss << node->key << "\n";
  indents[depth] = true;
  depth++;
  auto lastNode = node->maxNode();
  node->traverseInorder([&](const Node* iterNode){
    if (lastNode == iterNode) indents[depth - 1] = false;
    logNode(ss, iterNode, depth, indents);
  });
}

void FileSystem::logFile(std::stringstream& ss, const File* node, int depth, std::vector<bool>& indents) const {
  indent(ss, depth, indents);
  ss << node->key << "\n";
}

void FileSystem::logLink(std::stringstream & ss, const Link* node, int depth, std::vector<bool>& indents) const {
  indent(ss, depth, indents);
  ss << "link [" << node->key << "] \n";
}

const std::string& FileSystem::getLastError() {
  return gError;
}
