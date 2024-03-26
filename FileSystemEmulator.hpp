#pragma once

// WRITE LOG
// WRITE TESTS

// use tree for leafs in the node
// use 8byte + 3byte for the name of the node, also key in the tree
// don't store size in the tree
// better use RB tree

// each node type has different size
// implement node using virtual functions and inheritance
// only directory node has leafs (tree)

// make two classes - emulator and interpreter
// emulator stores only lower case names

// store number of incoming hard links in the node cache to ensure no hard-linked nodes can be removed


// convert command to lowercase then use full path as the key and update key pointer when descending to the leafs

// each node has parent pointer to check for current directory

// base node class has all the cache data. use it directly in the tree
// dynamic links ? -> yet another cache variable in each node?

#include "Path.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <filesystem>

typedef std::string Key;
typedef std::map<Key, struct Node*> Tree;

class Node {
public:
  Node() {
    type = NONE;
  }

  virtual void log(std::stringstream& ss, const Key& key, int depth) {
    ss << "ss";
  }

  virtual ~Node() = default;

protected:
  enum Type : unsigned int { NONE, DIRECTORY, FILE, LINK };
  Type type;
};

class File : public Node {
public:
  File() {
    type = Type::FILE;
  }

  void log(std::stringstream& ss, const Key& key, int depth) override {
    ss << std::setw(depth * 2) << key << "\n";
  }

  ~File() override = default;
};

class Link : public Node {
public:

  Link() {
    type = LINK;
  }

  void log(std::stringstream & ss, const Key& key, int depth) override {
    ss << std::setw(depth * 2) << "link [" << key << "] \n";
  }

  ~Link() override = default;

private:
  Node* link = nullptr;
  bool hard = false;
};

class Directory : public Node {
public:
  Directory() {
    type = DIRECTORY;
  }

  ~Directory() override = default;

  void detachNode(Node* node) {
    // TODO : remove util from avl tree
    // TODO : update all cache all the way up to the root (due to the links)
    // TODO : dont relocate nodes (due existing links to the nodes), only change tree pointers
  }

  bool attachNode(const Path& directoryPath, const Key& newKey, Node* newNode) {
    Node* node = findNode(directoryPath);
    if (!node) {
      // TODO : update error status - invalid path
      return false;
    }

    // if (node->type != DIRECTORY) {
      // TODO : update error status - given path is not a directory
      // return false;
    //}

    if (!((Directory*) node)->insertUtil(newKey, newNode)) return false;

    // if (newNode->type == LINK) {
      // TODO : update all caches (due to the new link update)
    // }

    return true;
  }

  bool insertUtil(const Key& newKey, Node* newNode) {
    // TODO : user avl tree insertion
    // TODO : check for existing file
    // TODO : return true if successful (node inserted)
    // TODO : dont relocate nodes (due existing links to the nodes), only change tree pointers
    return false;
  }

  Node* findNode(const Path& path) {
    Node* node = this;

    if (false /*!path.advance()*/) {
      return node;
    }

    const Key& key = {}; // path.getCurrentKey();
    node = subNodes.find(key)->second;

    if (!node) {
      return nullptr;
    }

    // TODO : dont allow cyclic links (with length one?)?
    while (true) {
      /*
      switch (node->type) {
        case Node::DIRECTORY:
          return ((Directory*)node)->findNode(path);

        case Node::LINK:
          node = ((Link*)node)->link;
          break;

        default:
          return nullptr;
      }
      */
    }
  }

  void log(std::stringstream & ss, const Key& key, int depth) override {
    ss << std::setw(depth * 2) << key;
    for (auto & node : subNodes) {
      node.second->log(ss, node.first, depth + 1);
    }
  }

private:
  Tree subNodes;
};

class FileSystem {
  Directory* root = nullptr;
  Directory* currentDirectory = nullptr;

public:
  FileSystem() {
    root = new Directory();
    initializeTransitions();
  }

  ~FileSystem() {
    delete root;
  }

  void makeDirectory(const Path& path) {
    Directory* parentDirectory = path.isAbsolute() ? root : currentDirectory;
    auto newDirectory = new Directory();
    /*
    if (!parentDirectory->attachNode(path.parent_path(), path.filename(), newDirectory)) {
      delete newDirectory;
    }
    */
  }

  void changeCurrent(const Path& path) {
    auto node = path.isAbsolute() ? root->findNode(path) : currentDirectory->findNode(path);
    // if (node->type != Node::DIRECTORY) {
      // TODO : update error status - no such directory
      // return;
    // }
    currentDirectory = (Directory*) node;
  }

  void log() {
    std::stringstream ss;
    root->log(ss, "/", 0);
    std::cout << ss.str();
  }
};