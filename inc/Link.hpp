#pragma once

#include "Node.hpp"

class Link : public Node {
public:
  Link(const std::shared_ptr<Node>& target, bool isHard);
  Link(const Link& node);
  ~Link() override;

  [[nodiscard]] std::shared_ptr<Node> clone() const override;

  [[nodiscard]] std::shared_ptr<Node> getLink() const;
  [[nodiscard]] bool isHard() const;

  std::shared_ptr<Node> getTarget() override;

  // link on link is not allowed, so no inf looping here
  std::shared_ptr<Node> findNode(const std::vector<Key>& path, ui32 currentDepth) override;

  void dumpUtil(std::stringstream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents) override;

  bool isLink() const override { return true; }

private:
  std::shared_ptr<Node> mLink = nullptr;
  bool mIsHard = false;
};