#pragma once

// use tree for leafs in the node
// use 8byte + 3byte for the name og the node, also key in the tree
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