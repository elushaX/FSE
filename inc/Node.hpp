#pragma once

typedef unsigned long long ui64;
typedef unsigned long ui32;
typedef long long i64;
typedef long i32;

#include <map>
#include <utility>
#include <vector>
#include <string>
#include <cassert>

class Link;

extern std::string gError;
typedef std::string Key;

class Node {
public:
  Node() = default;
  Node(const Node& node);
  virtual ~Node();

  [[nodiscard]] virtual Node* clone() const;

  virtual bool attachNode(const std::vector<Key>& directoryPath, const Key& newKey, Node* newNode) { return false; }
  virtual bool detachNode(const std::vector<Key>& directoryPath, const Key& key) { return false; }

  virtual bool detachNode(const Key& key) { return false; }
  virtual bool attachNode(const Key &newKey, Node *newNode) { return false; }

  virtual void getMaxDepthUtil(ui32 depth, ui32& maxDepth) const {}

  virtual void dumpUtil(std::stringstream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents);

  ui32 getMaxDepth() const;

  void dump(std::stringstream& ss);

  void getNodeStraightPath(Node* node, std::vector<const Node*>& path) const;

  virtual ui64 size() const { return 0; }

  virtual Node* getTarget() { return this; }

  virtual Node* findNode(const std::vector<Key>& path, ui32 currentDepth = 0);

  virtual Node* findNode(const Key& path) { return nullptr; }

  bool empty() const { return !size(); }

  static void indent(std::stringstream & ss, ui32 depth, std::vector<bool>& indents);

  virtual bool isDirectory() const { return false; }
  virtual bool isLink() const { return false; }
  virtual bool isHard() const { return false; }

public:
  Node* mParent = nullptr;

  std::vector<Link*> mIncomingHardLinks;
  std::vector<Link*> mIncomingDynamicLinks;
};