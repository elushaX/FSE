#pragma once

#include "Path.hpp"
#include <sstream>

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

  void updateTreeCache();
};

struct File : public Node {
  File();
};

struct Link : public Node {
  Link();

  Node* link = nullptr;
  bool hard = false;
};

struct Directory : public Node {
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

class FileSystem {
public:
  FileSystem();
  ~FileSystem();

  bool makeDirectory(const Path& path);
  bool changeCurrent(const Path& path);
  void log() const;

  const std::string& getLastError();

private:
  void logNode(std::stringstream& ss, const Node* node, int depth, std::vector<bool>& indents) const;
  void indent(std::stringstream & ss, int depth, std::vector<bool>& indents) const;
  void logDirectory(std::stringstream & ss, const Directory* node, int depth, std::vector<bool>& indents) const;
  void logFile(std::stringstream& ss, const File* node, int depth, std::vector<bool>& indents) const;
  void logLink(std::stringstream & ss, const Link* node, int depth, std::vector<bool>& indents) const;

private:
  Directory* root = nullptr;
  Directory* currentDirectory = nullptr;
};