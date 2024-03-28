#pragma once

#include "Tree.hpp"

#include <string>
#include <utility>
#include <vector>

extern std::string gError;
typedef std::string Key;

struct DirectoryKey {
  DirectoryKey() = default;
  explicit DirectoryKey(Key val) : val(std::move(val)) {}

  [[nodiscard]] inline bool descentRight(const DirectoryKey& in) const { return in.val > val; }
  [[nodiscard]] inline bool descentLeft(const DirectoryKey& in) const { return in.val < val; }
  [[nodiscard]] inline bool exactNode(const DirectoryKey& in) const { return in.val == val; }

  [[nodiscard]] inline const DirectoryKey& getFindKey() const { return *this; }
  static inline const DirectoryKey& keyInRightSubtree(const DirectoryKey& in) { return in; }
  static inline const DirectoryKey& keyInLeftSubtree(const DirectoryKey& in) { return in; }

  template <typename NodeType>
  inline void updateTreeCacheCallBack(NodeType& treeNode) {
    treeNode.data->mTreeNode = &treeNode;
    // TODO : update incoming links
  }

public:
  Key val;

  ui32 incomingLinksHard = 0;
  ui32 incomingLinksDynamic = 0;
};

typedef AvlTree<DirectoryKey, class Node*> DirectoryTree;

class Node {
public:
  enum Type : ui32 { NONE, DIRECTORY, FILE, LINK } ;

public:
  virtual ~Node();

public:
  Type mType = NONE;
  Node* mParent = nullptr;
  DirectoryTree::Node* mTreeNode = nullptr;
};

class File : public Node {
public:
  File();
};

class Link : public Node {
public:
  Link();
  [[nodiscard]] Node* getLink() const;

private:
  Node* mLink = nullptr;
  bool mIsHard = false;
};

class Directory : public Node {
public:
  Directory();
  ~Directory() override;

  void dump(std::stringstream& ss);

  bool attachNode(const std::vector<Key>& directoryPath, const Key& newKey, Node* newNode);
  bool detachNode(const std::vector<Key>& directoryPath, const Key& key);
  Node* findNode(const std::vector<Key>& path, ui32 currentDepth = 0);

  [[nodiscard]] ui32 getMaxDepth() const;

  template<typename tFunctor>
  void traverseInorder(tFunctor functor) const {
    mMembers.traverseInorder(mMembers.getRoot(), functor);
  }

  void getNodePath(Node* node, std::vector<const Key*>& path) const;
private:
  void updateTreeLinkCount(Node* node);
  void getMaxDepthUtil(ui32 depth, ui32& maxDepth) const;
  void dumpUtil(std::stringstream& ss, ui32 currentDepth, std::vector<bool>& indents);

public:
  DirectoryTree mMembers;
};