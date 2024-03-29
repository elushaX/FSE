#pragma once

#include "Node.hpp"

class Link : public Node {
public:
  Link(Node* target, bool isHard);
  Link(const Link& node);
  ~Link() override;

  [[nodiscard]] Link* clone() const override;

  [[nodiscard]] Node* getLink() const;
  [[nodiscard]] bool isHard() const;

  Node* getTarget() override;

  // link on link is not allowed, so no inf looping here
  Node* findNode(const std::vector<Key>& path, ui32 currentDepth) override;

  void dumpUtil(std::stringstream& ss, const Key& key, ui32 currentDepth, std::vector<bool>& indents) override;

  bool isLink() const override { return true; }

private:
  Node* mLink = nullptr;
  bool mIsHard = false;
};