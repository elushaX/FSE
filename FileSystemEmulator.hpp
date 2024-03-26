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

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <filesystem>

typedef std::filesystem::path Path;
typedef std::string Key;

struct Node {
  Key key;

  Node* left = nullptr;
  Node* right = nullptr;
  Node* parent = nullptr;
  unsigned int height = 0;

  unsigned int inLinks = 0;
  unsigned int outLinks = 0;

  enum Type : unsigned int { NONE, DIRECTORY, FILE, LINK } type = NONE;
};

struct File : Node {
  File() {
    type = FILE;
  }
};

struct Link : Node {
  Node* link = nullptr;
  bool hard = false;

  Link() {
    type = LINK;
  }
};

struct Directory : Node {
  Node subNodes;

  Directory() {
    type = DIRECTORY;
  }


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

    if (node->type != DIRECTORY) {
      // TODO : update error status - given path is not a directory
      return false;
    }

    if (!((Directory*) node)->insertUtil(newKey, newNode)) return false;

    if (newNode->type == LINK) {
      // TODO : update all caches (due to the new link update)
    }

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
    node = findUtil(key, &subNodes);

    if (!node) {
      return nullptr;
    }

    // TODO : dont allow cyclic links (with length one?)?
    while (true) {
      switch (node->type) {
        case Node::DIRECTORY:
          return ((Directory*)node)->findNode(path);

        case Node::LINK:
          node = ((Link*)node)->link;
          break;

        default:
          return nullptr;
      }
    }
  }

  Node* findUtil(const Key& key, Node* node) {
    Node* iter = node;
    while (true) {
      if (!iter) return nullptr;
      if (iter->key == key) return iter;
      if (key > iter->key) {
        iter = iter->left;
      } else {
        iter = iter->right;
      }
    }
  }

};

class FileSystem {
  Directory* root = nullptr;
  Directory* currentDirectory = nullptr;

public:
  FileSystem() {
    root = new Directory();
  }

  ~FileSystem() {
    delete root;
  }

  void makeDirectory(const Path& path) {
    Directory* parentDirectory = path.is_relative() ? currentDirectory : root;
    auto newDirectory = new Directory();
    if (!parentDirectory->attachNode(path.parent_path(), path.filename(), newDirectory)) {
      delete newDirectory;
    }
  }

  void changeCurrent(const char* directoryString) {
    auto path = Path(directoryString);
    auto node = path.is_relative() ? root->findNode(path) : currentDirectory->findNode(path);
    if (node->type != Node::DIRECTORY) {
      // TODO : update error status - no such directory
      return;
    }
    currentDirectory = (Directory*) node;
  }

  void log() {
    std::stringstream ss;
    logNode(ss, root, 0);
    std::cout << ss.str();
  }

private:
  void logUtil(std::stringstream& ss, const Node* node, int depth) {
    if (!node) return;
    logUtil(ss, node->left, depth);
    logNode(ss, node, depth);
    logUtil(ss, node->right, depth);
  }

  void logNode(std::stringstream& ss, const Node* node, int depth) {
    switch (node->type) {
      case Node::DIRECTORY:
        return logDirectory(ss, (Directory*) node, depth);
      case Node::FILE:
        return logFile(ss, (File*) node, depth);
      case Node::LINK:
        return logLink(ss, (Link*) node, depth);
      default:
        exit(1);
    }
  }

  void logDirectory(std::stringstream& ss, const Directory* dir, int depth) {
    ss << std::setw(depth * 2) << dir->key;
    logUtil(ss, &dir->subNodes, depth + 1);
  }

  void logFile(std::stringstream& ss, const File* file, int depth) {
    ss << std::setw(depth * 2) << file->key << "\n";
  }

  void logLink(std::stringstream& ss, const Link* link, int depth) {
    ss << std::setw(depth * 2) << "link [" << link->key << "] \n";
  }
};