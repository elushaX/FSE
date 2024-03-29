#pragma once

#include "Node.hpp"

class Link : public Node {
public:
  Link();
  Link(const Link& node);
  ~Link() override;

  NodeType getType() const override { return LINK; }
  [[nodiscard]] std::shared_ptr<Node> clone() const override;
  [[nodiscard]] std::shared_ptr<Node> getLink() const;
  [[nodiscard]] bool isHard() const;

  std::shared_ptr<Node> getTarget() override;

  // link on link is not allowed, so no inf looping here
  std::shared_ptr<Node> findNode(const std::vector<Key>& path, ui32 currentDepth) override;
  void dumpUtil(std::stringstream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents) override;

  static bool linkNodes(const std::shared_ptr<Link>& link, const std::shared_ptr<Node>& target, bool hard);

private:
  std::weak_ptr<Node> mLink;
  bool mIsHard = false;
};