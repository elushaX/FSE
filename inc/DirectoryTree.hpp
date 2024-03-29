#pragma once

typedef unsigned long long ui64;
typedef unsigned long ui32;
typedef long long i64;
typedef long i32;

#include <map>
#include <utility>
#include <vector>
#include <string>

class Link;

extern std::string gError;
typedef std::string Key;

class Node {
public:
  Node() = default;
  Node(const Node& node);
  virtual ~Node();

  [[nodiscard]] virtual Node* clone() const;

public:
  enum Type : ui32 { NONE, DIRECTORY, FILE, LINK } ;
  Type mType = NONE; // TODO : remove

  class Directory* mParent = nullptr;

  std::vector<Link*> mIncomingHardLinks;
  std::vector<Link*> mIncomingDynamicLinks;
};

typedef std::map<Key, Node*> DirectoryTree;

class File : public Node {
public:
  File();
  File(const File& node);

  [[nodiscard]] File* clone() const override;
};

class Link : public Node {
public:
  Link(Node* target, bool isHard);
  Link(const Link& node);
  ~Link() override;

  [[nodiscard]] Link* clone() const override;

  [[nodiscard]] Node* getLink() const;
  [[nodiscard]] bool isHard() const;

private:
  Node* mLink = nullptr;
  bool mIsHard = false;
};

class Directory : public Node {
public:
  Directory();
  Directory(const Directory& node);

  ~Directory() override;

  [[nodiscard]] Directory* clone() const override;

  void dump(std::stringstream& ss);

  bool attachNode(const std::vector<Key>& directoryPath, const Key& newKey, Node* newNode);
  bool detachNode(const std::vector<Key>& directoryPath, const Key& key);

  bool detachNode(const Key& key);
  bool attachNode(const Key &newKey, Node *newNode);

  Node* findNode(const std::vector<Key>& path, ui32 currentDepth = 0);
  Node* findNode(const Key& path);

  [[nodiscard]] ui32 getMaxDepth() const;

  void getNodeStraightPath(Node* node, std::vector<const Node*>& path) const;
  [[nodiscard]] ui64 size() const;

private:
  void getMaxDepthUtil(ui32 depth, ui32& maxDepth) const;
  void dumpUtil(std::stringstream& ss, ui32 currentDepth, std::vector<bool>& indents);

public:
  DirectoryTree mMembers;
};