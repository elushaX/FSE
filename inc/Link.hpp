#pragma once

#include "Node.hpp"

class Link : public Node {
public:
  Link();
  Link(const Link& node);
  ~Link() override;

  [[nodiscard]] std::shared_ptr<Node> clone() const override;

  std::shared_ptr<Node> findNode(const std::vector<Key>& path, ui32 currentDepth) override;

  [[nodiscard]] bool isHardLink() const;
  static bool linkNodes(const std::shared_ptr<Link>& link, const std::shared_ptr<Node>& target, bool hard);
  void removeOutgoingLinks() override;

  std::shared_ptr<Node> getTarget() override;
  [[nodiscard]] std::shared_ptr<Node> getLink() const;
  [[nodiscard]] NodeType getType() const override { return LINK; }
  void dumpUtil(std::ostream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents) override;

private:
  std::weak_ptr<Node> mLink;
  bool mIsHard = false;
};