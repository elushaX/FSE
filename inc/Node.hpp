#pragma once

#include <map>
#include <utility>
#include <vector>
#include <string>
#include <cassert>
#include <memory>

typedef unsigned long long ui64;
typedef unsigned long ui32;
typedef std::string Key;

class Link;
class Directory;

class Node {
public:
  enum NodeType { FILE, LINK, DIRECTORY };

public:
  Node() = default;
  Node(const Node& node);
  virtual ~Node();

  // modification
  virtual bool detachNode(const Key& key);
  virtual bool attachNode(const Key &newKey, const std::shared_ptr<Node>& newNode);
  virtual std::shared_ptr<Node> findNode(const std::vector<Key>& path, ui32 currentDepth);
  virtual std::shared_ptr<Node> findNode(const Key& path);
  [[nodiscard]] virtual std::shared_ptr<Node> clone() const;

  // logging
  std::ostream& dump(std::ostream &stream);
  virtual void dumpUtil(std::ostream& stream, const Key& key, ui32 currentDepth, std::vector<bool>& indents);
  static void indent(std::ostream& ss, ui32 depth, std::vector<bool>& indents);

  // link support
  virtual void clearFlags(std::shared_ptr<Node>& directory);
  virtual void removeIncomingDynamicLinks();
  virtual void removeOutgoingLinks();
  virtual std::shared_ptr<Node> getTarget();
  [[nodiscard]] virtual bool isHardNode() const;

  // general queries
  [[nodiscard]] virtual NodeType getType() const;
  [[nodiscard]] virtual ui64 size() const;
  [[nodiscard]] ui32 getMaxDepth() const;
  virtual void getMaxDepthUtil(ui32 depth, ui32& maxDepth) const;
  void getNodeStraightPath(const std::shared_ptr<Node>& node, std::vector<std::shared_ptr<Node>>& path) const;
  [[nodiscard]] bool empty() const;

public:
  // TODO : remove key duplication
  Key mKey;

  // to mark currently working subtree recursively.
  std::weak_ptr<Node> mWorkingNodeFlag;

  // to trace path from current node to the root
  std::weak_ptr<Node> mParent;

  // to remove incoming links
  std::vector<std::weak_ptr<Link>> mIncomingLinks;
};