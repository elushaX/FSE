#pragma once

#include "Node.hpp"

class Directory : public Node {
  typedef std::map<Key, std::shared_ptr<Node>> DirectoryTree;

public:
  Directory();
  Directory(const Directory& node);
  ~Directory() override;

  bool detachNode(const Key& key) override;
  bool attachNode(const Key &newKey, const std::shared_ptr<Node>& newNode) override;
  std::shared_ptr<Node> findNode(const std::vector<Key>& path, ui32 currentDepth) override;
  std::shared_ptr<Node> findNode(const Key& path) override;

  [[nodiscard]] std::shared_ptr<Node> clone() const override;

  void clearFlags(std::shared_ptr<Node>& directory) override;
  [[nodiscard]] bool isHardNode() const override;
  void removeIncomingDynamicLinks() override;
  void removeOutgoingLinks() override;

  [[nodiscard]] NodeType getType() const override { return DIRECTORY; }
  [[nodiscard]] ui64 size() const override;
  void getMaxDepthUtil(ui32 depth, ui32& maxDepth) const override;
  void dumpUtil(std::ostream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents) override;

  template <typename tFunctor>
  static void traverse(std::shared_ptr<Node>& node, tFunctor functor) {
    functor(node);
    if (std::shared_ptr<Directory> directory = std::dynamic_pointer_cast<Directory>(node)) {
      for (auto& member : directory->mMembers) {
        traverse(member.second, functor);
      }
    }
  }

public:
  DirectoryTree mMembers;
};