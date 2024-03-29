#pragma once

#include "Node.hpp"

class Directory : public Node {
  typedef std::map<Key, Node*> DirectoryTree;

public:
  Directory();
  Directory(const Directory& node);

  ~Directory() override;

  [[nodiscard]] Directory* clone() const override;

  bool attachNode(const std::vector<Key>& directoryPath, const Key& newKey, Node* newNode) override;
  bool detachNode(const std::vector<Key>& directoryPath, const Key& key) override;

  bool detachNode(const Key& key) override;
  bool attachNode(const Key &newKey, Node *newNode) override;

  Node* findNode(const std::vector<Key>& path, ui32 currentDepth = 0) override;
  Node* findNode(const Key& path) override;

  [[nodiscard]] ui64 size() const override;

  bool isDirectory() const override { return true; }

private:
  void getMaxDepthUtil(ui32 depth, ui32& maxDepth) const;
  void dumpUtil(std::stringstream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents) override;

public:
  DirectoryTree mMembers;
};