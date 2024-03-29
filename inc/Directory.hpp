#pragma once

#include "Node.hpp"

class Directory : public Node {
  typedef std::map<Key, std::shared_ptr<Node>> DirectoryTree;

public:
  Directory();
  Directory(const Directory& node);

  ~Directory() override;

  [[nodiscard]] std::shared_ptr<Node> clone() const override;

  bool detachNode(const Key& key) override;
  bool attachNode(const Key &newKey, std::shared_ptr<Node> newNode) override;

  std::shared_ptr<Node> findNode(const std::vector<Key>& path, ui32 currentDepth = 0) override;
  std::shared_ptr<Node> findNode(const Key& path) override;

  [[nodiscard]] ui64 size() const override;

  bool isDirectory() const override { return true; }

private:
  void getMaxDepthUtil(ui32 depth, ui32& maxDepth) const;
  void dumpUtil(std::stringstream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents) override;

public:
  DirectoryTree mMembers;
};