#pragma once

#include <cassert>
#include <algorithm>

typedef unsigned long long ui64;
typedef unsigned long ui32;
typedef long long i64;
typedef long i32;

template <typename T>
struct SelectValueOrReference {
  using type = typename std::conditional<std::is_scalar<T>::value, T, const T&>::type;
};

template <typename NumericType>
struct AvlNumericKey {

  NumericType val;

  AvlNumericKey() = default;
  AvlNumericKey(NumericType val) :
    val(val) {}

  inline bool descentRight(AvlNumericKey in) const { return in.val > val; }
  inline bool descentLeft(AvlNumericKey in) const { return in.val < val; }
  inline bool exactNode(AvlNumericKey in) const { return in.val == val; }

  inline const AvlNumericKey& getFindKey(/**/) const { return *this; }
  inline const AvlNumericKey& keyInRightSubtree(const AvlNumericKey& in) const { return in; }
  inline const AvlNumericKey& keyInLeftSubtree(const AvlNumericKey& in) const { return in; }

  template <typename NodeType>
  inline void updateTreeCacheCallBack(const NodeType&) {}
};

template <typename Key, typename Data>
class AvlTree {
  typedef SelectValueOrReference<Key>::type KeyArg;
  typedef SelectValueOrReference<Data>::type DataArg;

public:
  class Node {
    friend AvlTree;

  private:
    Node(KeyArg aKey, DataArg aData) :
      key(aKey),
      data(aData) {}

  public:
    Data data;
    Key key;

  public:
    Node* mLeft = nullptr;
    Node* mRight = nullptr;
    Node* mParent = nullptr;
    ui64 mHeight = 0;

  private:
    inline bool descentRight(KeyArg aKey) const { return key.descentRight(aKey); }
    inline bool descentLeft(KeyArg aKey) const { return key.descentLeft(aKey); }
    inline bool exactNode(KeyArg aKey) const { return key.exactNode(aKey); }

    inline KeyArg getFindKey(const Node* node = nullptr) const { return key.getFindKey(/*node*/); }
    inline KeyArg keyInRightSubtree(KeyArg aKey) const { return key.keyInRightSubtree(aKey); }
    inline KeyArg keyInLeftSubtree(KeyArg aKey) const { return key.keyInLeftSubtree(aKey); }

    inline void updateTreeCacheCallBack() { key.updateTreeCacheCallBack(*this); }
  };

public:
  AvlTree() = default;
  ~AvlTree() { removeAll(); }

  [[nodiscard]] ui64 size() const { return mSize; }

  Node* getRoot() const { return this->mRoot; }

  void insert(KeyArg key, DataArg data) {
    mRoot = insertUtil(mRoot, key, data);
    mRoot->mParent = nullptr;
  }

  void remove(KeyArg key) {
    mRoot = removeUtil(mRoot, key);
    if (mRoot) mRoot->mParent = nullptr;
  }

  Node* maxNode(Node* head) const {
    if (!head) return nullptr;
    while (head->mRight != nullptr) {
      head = head->mRight;
    }
    return head;
  }

  Node* minNode(Node* head) const {
    if (!head) return nullptr;
    while (head->mLeft != nullptr) {
      head = head->mLeft;
    }
    return head;
  }

  Node* find(KeyArg key) const {
    Node* iter = mRoot;
    while (true) {
      if (!iter) return nullptr;
      if (iter->exactNode(key)) return iter;
      if (iter->descentLeft(key)) {
        // key = iter->keyInLeftSubtree(key);
        iter = iter->mLeft;
      } else {
        // key = iter->keyInRightSubtree(key);
        iter = iter->mRight;
      }
    }
  }

  Node* findLessOrEq(KeyArg key) const {
    Node* iter = mRoot;
    while (true) {
      if (!iter) return nullptr;
      if (iter->exactNode(key)) return iter;
      if (iter->descentLeft(key)) {
        if (iter->mLeft) {
          key = iter->keyInLeftSubtree(key);
          iter = iter->mLeft;
        } else {
          return iter;
        }
      } else {
        if (iter->mRight) {
          key = iter->keyInRightSubtree(key);
          iter = iter->mRight;
        } else {
          return iter;
        }
      }
    }
  }

  // returns first invalid node
  const Node* findInvalidNode(const Node* head) const {
    if (head == nullptr) return nullptr;

    if (head->mLeft) {
      // TODO: incomplete test
      if (!head->descentLeft(head->mLeft->getFindKey(head))) return head;
      if (head->mLeft->mParent != head) return head;
      if (!head->mRight && head->mLeft->mHeight != head->mHeight - 1) return head;
    }

    if (head->mRight) {
      if (!head->descentRight(head->mRight->getFindKey(head))) return head;
      if (head->mRight->mParent != head) return head;
      if (!head->mLeft && head->mRight->mHeight != head->mHeight - 1) return head;
    }

    if (head->mLeft && head->mRight) {
      if (std::max(head->mLeft->mHeight, head->mRight->mHeight) != head->mHeight - 1) return head;
    }

    int balance = getNodeHeight(head->mRight) - getNodeHeight(head->mLeft);

    if (balance > 1 || balance < -1) return head;

    const Node* ret = findInvalidNode(head->mRight);

    if (ret) return ret;

    return findInvalidNode(head->mLeft);
  }

  bool isValid() { return findInvalidNode(getRoot()) == nullptr; }

  template <typename tFunctor>
  void traverseInorder(Node* node, tFunctor functor) {
    if (!node) return;
    traverseInorder(node->mLeft, functor);
    functor(node);
    traverseInorder(node->mRight, functor);
  }

  template <typename tFunctor>
  void traverseInorder(const Node* node, tFunctor functor) const {
    if (!node) return;
    traverseInorder(node->mLeft, functor);
    functor(node);
    traverseInorder(node->mRight, functor);
  }

  void removeAll() {
    if (!mRoot) return;
    removeUtil(mRoot);
    mRoot = nullptr;
    mSize = 0;
  }

  void removeUtil(Node* node) {
    if (node->mLeft) removeUtil(node->mLeft);
    if (node->mRight) removeUtil(node->mRight);
    deleteNode(node);
  }

private:
  inline void deleteNode(Node* node) {
    delete node;
  }

  inline Node* newNode(KeyArg key, DataArg data) {
    auto out = new Node(key, data);
    return out;
  }

  inline void injectNodeInstead(Node* target, Node* from) {
    std::swap(target->data, from->data);
    target->key = from->key;
    target->updateTreeCacheCallBack();
    from->updateTreeCacheCallBack();
  }

  inline i64 getNodeHeight(const Node* node) const { return node ? node->mHeight : -1; }

  // returns new head
  Node* rotateLeft(Node* pivot) {
    assert(pivot);

    Node* const head = pivot;
    Node* const right = pivot->mRight;
    Node* const right_left = right->mLeft;
    Node* const parent = pivot->mParent;

    // parents
    if (right_left) right_left->mParent = head;
    head->mParent = right;
    right->mParent = parent;

    // children
    head->mRight = right_left;
    right->mLeft = head;

    // heights
    head->mHeight = 1 + std::max(getNodeHeight(head->mLeft), getNodeHeight(head->mRight));
    right->mHeight = 1 + std::max(getNodeHeight(right->mLeft), getNodeHeight(right->mRight));

    // cache
    head->updateTreeCacheCallBack();
    right->updateTreeCacheCallBack();

    return right;
  }

  Node* rotateRight(Node* pivot) {
    assert(pivot);

    Node* const head = pivot;
    Node* const left = pivot->mLeft;
    Node* const left_right = left->mRight;
    Node* const parent = pivot->mParent;

    // parents
    if (left_right) left_right->mParent = head;
    head->mParent = left;
    left->mParent = parent;

    // children
    head->mLeft = left_right;
    left->mRight = head;

    // heights
    head->mHeight = 1 + std::max(getNodeHeight(head->mLeft), getNodeHeight(head->mRight));
    left->mHeight = 1 + std::max(getNodeHeight(left->mLeft), getNodeHeight(left->mRight));

    // cache
    head->updateTreeCacheCallBack();
    left->updateTreeCacheCallBack();

    return left;
  }

  // recursively returns valid isLeft or isRight child or root
  Node* insertUtil(Node* head, KeyArg key, DataArg data) {

    Node* insertedNode;

    if (head == nullptr) {
      mSize++;
      Node* out = newNode(key, data);
      out->updateTreeCacheCallBack();
      return out;
    } else if (head->exactNode(key)) {
      return head;
    } else if (head->descentRight(key)) {
      insertedNode = insertUtil(head->mRight, head->keyInRightSubtree(key), data);
      head->mRight = insertedNode;
      insertedNode->mParent = head;
    } else {
      insertedNode = insertUtil(head->mLeft, head->keyInLeftSubtree(key), data);
      head->mLeft = insertedNode;
      insertedNode->mParent = head;
    }

    // update height
    head->mHeight = 1 + std::max(getNodeHeight(head->mRight), getNodeHeight(head->mLeft));

    i64 balance = i64(getNodeHeight(head->mRight) - getNodeHeight(head->mLeft));

    if (balance > 1) {
      if (head->mRight->descentRight(head->keyInRightSubtree(key))) {
        return rotateLeft(head);
      } else {
        head->mRight = rotateRight(head->mRight);
        return rotateLeft(head);
      }
    } else if (balance < -1) {
      if (head->mLeft->descentLeft(head->keyInLeftSubtree(key))) {
        return rotateRight(head);
      } else {
        head->mLeft = rotateLeft(head->mLeft);
        return rotateRight(head);
      }
    }

    head->updateTreeCacheCallBack();

    return head;
  }

  Node* removeUtil(Node* head, KeyArg key) {
    if (head == nullptr) return head;

    if (head->exactNode(key)) {
      if (head->mRight && head->mLeft) {
        Node* min = minNode(head->mRight);
        auto const& newKey = min->getFindKey(head->mRight);
        injectNodeInstead(head, min);
        head->mRight = removeUtil(head->mRight, newKey);
      } else if (head->mRight) {
        injectNodeInstead(head, head->mRight);
        deleteNode(head->mRight);
        head->mRight = nullptr;
        mSize--;
      } else if (head->mLeft) {
        injectNodeInstead(head, head->mLeft);
        deleteNode(head->mLeft);
        head->mLeft = nullptr;
        mSize--;
      } else {
        deleteNode(head);
        mSize--;
        head = nullptr;
      }
    } else if (head->descentRight(key)) {
      head->mRight = removeUtil(head->mRight, head->keyInRightSubtree(key));
    } else if (head->descentLeft(key)) {
      head->mLeft = removeUtil(head->mLeft, head->keyInLeftSubtree(key));
    }

    if (head == nullptr) return head;

    head->mHeight = 1 + std::max(getNodeHeight(head->mRight), getNodeHeight(head->mLeft));
    i64 balance = getNodeHeight(head->mRight) - getNodeHeight(head->mLeft);

    if (balance < -1) {
      if (getNodeHeight(head->mLeft->mLeft) >= getNodeHeight(head->mLeft->mRight)) {
        return rotateRight(head);
      } else {
        head->mLeft = rotateLeft(head->mLeft);
        return rotateRight(head);
      }
    } else if (balance > 1) {
      if (getNodeHeight(head->mRight->mRight) >= getNodeHeight(head->mRight->mLeft)) {
        return rotateLeft(head);
      } else {
        head->mRight = rotateRight(head->mRight);
        return rotateLeft(head);
      }
    }

    head->updateTreeCacheCallBack();

    return head;
  }

private:
  Node* mRoot = nullptr;
  ui64 mSize = 0;
};
