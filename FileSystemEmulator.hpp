#pragma once

#include "Path.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <filesystem>
#include <cmath>

typedef std::string Key;
typedef unsigned long long ui64;
typedef unsigned long long ui32;

struct Node {
  enum Type : ui32 { NONE, DIRECTORY, FILE, LINK };

  Key key;

  Node* parent = nullptr;
  Node* left = nullptr;
  Node* right = nullptr;

  ui32 height = 0;
  ui32 incomingLinksHard = 0;
  ui32 incomingLinksDynamic = 0;

  Type type = NONE;

  void updateTreeCache() {
    // TODO update cache
  }
};

struct File : public Node {
  File() {
    type = Type::FILE;
  }
};

struct Link : public Node {
  Link() {
    type = LINK;
  }

  Node* link = nullptr;
  bool hard = false;
};

struct Directory : public Node {
  Node* members = nullptr;
  ui32 size = 0;

  Directory() {
    type = DIRECTORY;
  }

  bool attachNode(const std::vector<Key>& directoryPath, const Key& newKey, Node* newNode) {
    Node* node = findNode(directoryPath, 0);
    if (!node) {
      // TODO : update error status - invalid path
      return false;
    }

    if (node->type != DIRECTORY) {
      // TODO : update error status - given path is not a directory
      return false;
    }

    auto directory = ((Directory*) node);

    Node* existingNode = directory->treeSearch(newKey);
    if (existingNode) {
      if (existingNode->type == newNode->type) return false; // exit silently
      // TODO : report error cant add node
      return false;
    }

    directory->treeInsert(newKey, newNode);
    updateTreeLinkCount(newNode);

    return true;
  }

  Node* findNode(const std::vector<Key>& path, ui32 currentDepth) {
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

  Node* treeSearch(const Key& key) {
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

  void updateTreeLinkCount(Node* node) {
    // TODO : update all caches all the way up to '/'
  }

  void detachNode(Node* node) {
    // TODO : remove util from avl tree
    // TODO : update all cache all the way up to the root (due to the links)
    // TODO : dont relocate nodes (due existing links to the nodes), only change tree pointers
  }

  // TODO : user avl tree insertion
  // TODO : check for existing file
  // TODO : return true if successful (node inserted)
  // TODO : dont relocate nodes (due existing links to the nodes), only change tree pointers
  void treeInsert(const Key& newKey, Node* newNode) {
    newNode->key = newKey;
    members = insertUtil(members, newKey, newNode);
  }

  // recursively returns valid isLeft or isRight child or root
  Node* insertUtil(Node* head, const Key& key, Node* aNode) {

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
  Node* rotateLeft(Node* pivot) {
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

  Node* rotateRight(Node* pivot) {
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

  static inline ui32 getNodeHeight(const Node* node) { return node ? node->height : -1; }

  template<typename tFunctor>
  void traverseInorder(tFunctor functor) const {
    traverseInorderUtil(members, functor);
  }

  template<typename tFunctor>
  void traverseInorderUtil(Node* node, tFunctor functor) const {
    if (!node) return;
    traverseInorderUtil(node->left, functor);
    functor(node);
    traverseInorderUtil(node->right, functor);
  }

  Node* maxNode() const {
    Node* head = members;
    if (!head) return nullptr;
    while (head->right != nullptr) {
      head = head->right;
    }
    return head;
  }

  void getMaxDepthUtil(ui32 depth, ui32& maxDepth) const {
    if (!members) return;
    maxDepth = std::max(depth, maxDepth);
    traverseInorderUtil(members, [&](Node* node){
      if (node->type == DIRECTORY) {
        ((Directory*)node)->getMaxDepthUtil(++depth, maxDepth);
      }
    });
  }

  ui32 getMaxDepth() const {
    ui32 maxDepth = 0;
    getMaxDepthUtil(0, maxDepth);
    return maxDepth;
  }
};

class FileSystem {
  Directory* root = nullptr;
  Directory* currentDirectory = nullptr;

public:
  FileSystem() {
    root = new Directory();
    currentDirectory = root;
    root->key = "/";
    initializeTransitions();
  }

  ~FileSystem() {
    delete root;
  }

  void makeDirectory(const Path& path) {
    if (path.getDepth() < 1 || path.isInvalid()) {
      // TODO : update error - invalid path
      return;
    }

    Directory* parentDirectory = path.isAbsolute() ? root : currentDirectory;
    auto newDirectory = new Directory();
    if (!parentDirectory->attachNode(path.getParentChain(), path.getFilename(), newDirectory)) {
      delete newDirectory;
    }
  }

  void changeCurrent(const Path& path) {
    if (path.isInvalid()) {
      // TODO : update error - invalid path
      return;
    }

    auto node = path.isAbsolute() ? root->findNode(path.getChain(), 0) : currentDirectory->findNode(path.getChain(), 0);
    if (node->type != Node::DIRECTORY) {
      // TODO : update error status - no such directory
      return;
    }
    currentDirectory = (Directory*) node;
  }

  void log() const {
    std::stringstream ss;
    std::vector<bool> indents;
    indents.resize(root->getMaxDepth());
    logNode(ss, root, 0, indents);
    std::cout << ss.str();
  }

private:
  void logNode(std::stringstream& ss, const Node* node, int depth, std::vector<bool>& indents) const {
    switch (node->type) {
      case Node::DIRECTORY:
        return logDirectory(ss, (Directory*) node, depth, indents);
      case Node::FILE:
        return logFile(ss, (File*) node, depth, indents);
      case Node::LINK:
        return logLink(ss, (Link*) node, depth, indents);
    }
  }

  void indent(std::stringstream & ss, int depth, std::vector<bool>& indents) const {
    if (!depth) return;
    for (auto i  = 0; i < depth - 1; i++) {
      ss << (indents[i] ? " |" : "  ");
    }
    ss << " |_";
  }

  void logDirectory(std::stringstream & ss, const Directory* node, int depth, std::vector<bool>& indents) const {
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

  void logFile(std::stringstream& ss, const File* node, int depth, std::vector<bool>& indents) const {
    indent(ss, depth, indents);
    ss << node->key << "\n";
  }

  void logLink(std::stringstream & ss, const Link* node, int depth, std::vector<bool>& indents) const {
    indent(ss, depth, indents);
    ss << "link [" << node->key << "] \n";
  }
};