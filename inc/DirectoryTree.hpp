#pragma once

#include <string>
#include <vector>

typedef unsigned long long ui64;
typedef unsigned long long ui32;

typedef std::string Key;
extern std::string gError;

class Node {
public:
  enum Type : ui32 { NONE, DIRECTORY, FILE, LINK };

  Key key;

  Node* parent = nullptr;
  Node* left = nullptr;
  Node* right = nullptr;

  ui32 height = 0;
  ui32 incomingLinksHard = 0;
  ui32 incomingLinksDynamic = 0;

  Type type = NONE;

  void updateTreeCache();
};

class File : public Node {
public:
  File();
};

class Link : public Node {
public:
  Link();

  Node* link = nullptr;
  bool hard = false;
};

class Directory : public Node {
public:
  Directory();

  bool attachNode(const std::vector<Key>& directoryPath, const Key& newKey, Node* newNode);
  Node* findNode(const std::vector<Key>& path, ui32 currentDepth = 0);
  void detachNode(Node* node);

  template<typename tFunctor>
  void traverseInorder(tFunctor functor) const {
    traverseInorderUtil(members, functor);
  }

  [[nodiscard]] Node* maxNode() const;
  [[nodiscard]] ui32 getMaxDepth() const;

private:
  Node* treeSearch(const Key& key);
  void updateTreeLinkCount(Node* node);
  void treeInsert(const Key& newKey, Node* newNode);
  Node* insertUtil(Node* head, const Key& key, Node* aNode);
  Node* rotateLeft(Node* pivot);
  Node* rotateRight(Node* pivot);
  static inline ui32 getNodeHeight(const Node* node);
  void getMaxDepthUtil(ui32 depth, ui32& maxDepth) const;

  template<typename tFunctor>
  void traverseInorderUtil(Node* node, tFunctor functor) const {
    if (!node) return;
    traverseInorderUtil(node->left, functor);
    functor(node);
    traverseInorderUtil(node->right, functor);
  }

public:
  Node* members = nullptr;
  ui32 size = 0;
};