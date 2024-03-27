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
  inline void updateTreeCacheCallBack(const NodeType&) {
    // TODO : update incoming links
  }

public:
  Key val;

  ui32 incomingLinksHard = 0;
  ui32 incomingLinksDynamic = 0;
};

class Node {
public:
  enum Type : ui32 { NONE, DIRECTORY, FILE, LINK } ;

public:
  virtual void dump(const Node* node, const Key& key) const = 0;
  virtual ~Node();

public:
  Type mType = NONE;
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
  typedef AvlTree<DirectoryKey, Node*> DirectoryTree;

public:
  Directory();
  ~Directory() override;

  bool attachNode(const std::vector<Key>& directoryPath, const Key& newKey, Node* newNode);
  Node* findNode(const std::vector<Key>& path, ui32 currentDepth = 0);
  void detachNode(Node* node);

  [[nodiscard]] ui32 getMaxDepth() const;

  template<typename tFunctor>
  void traverseInorder(tFunctor functor) const {
    mMembers.traverseInorder(mMembers.getRoot(), functor);
  }

private:
  void updateTreeLinkCount(Node* node);
  void getMaxDepthUtil(ui32 depth, ui32& maxDepth) const;

public:
  DirectoryTree mMembers;
};