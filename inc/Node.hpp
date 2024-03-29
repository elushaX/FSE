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
#include <memory>

class Link;

typedef std::string Key;

class Node {
public:
  enum NodeType { FILE, LINK, DIRECTORY };

public:
  Node() = default;
  Node(const Node& node);
  virtual ~Node();

  virtual NodeType getType() const { return FILE; }
  virtual std::shared_ptr<Node> clone() const;
  virtual bool detachNode(const Key& key) { return false; }
  virtual bool attachNode(const Key &newKey, std::shared_ptr<Node> newNode) { return false; }
  virtual void getMaxDepthUtil(ui32 depth, ui32& maxDepth) const {}
  virtual void dumpUtil(std::stringstream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents);
  virtual ui64 size() const { return 0; }
  virtual std::shared_ptr<Node> getTarget();
  virtual std::shared_ptr<Node> findNode(const std::vector<Key>& path, ui32 currentDepth = 0);
  virtual std::shared_ptr<Node> findNode(const Key& path) { return nullptr; }
  virtual bool isDirectory() const { return false; }
  virtual bool isLink() const { return false; }
  virtual bool isHard() const { return false; }

  ui32 getMaxDepth() const;
  void dump(std::stringstream& ss);
  void getNodeStraightPath(const std::shared_ptr<Node>& node, std::vector<std::shared_ptr<Node>>& path) const;
  bool empty() const { return !size(); }
  static void indent(std::stringstream & ss, ui32 depth, std::vector<bool>& indents);

public:
  std::weak_ptr<Node> mParent;

  std::vector<std::weak_ptr<Link>> mIncomingHardLinks;
  std::vector<std::weak_ptr<Link>> mIncomingDynamicLinks;
};