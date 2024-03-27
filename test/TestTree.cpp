#include "Tree.hpp"

#include "UnitTest++/UnitTest++.h"

static double randomFloat() {
  return static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX);
}

const auto size = 10000;

class TestClass {
  ui64 val1 = 0;

public:
  TestClass() = default;
  explicit TestClass(ui64 val) : val1(val) {}
  [[nodiscard]] bool operator==(const TestClass& in) const { return in.val1 == val1; }
  [[nodiscard]] ui64 getVal() const { return val1; }
  void setVal(ui64 val) { val1 = val; }
};

SUITE(AvlTree) {
    TEST(Simple) {
      AvlTree<AvlNumericKey<i64>, TestClass> tree;

      CHECK(tree.size() == 0);
      CHECK(tree.getRoot() == nullptr);

      tree.insert(6, TestClass(6));
      CHECK(tree.isValid());
      CHECK(tree.size() == 1);
      CHECK(tree.getRoot()->data == TestClass(6));

      tree.remove(6);
      CHECK(tree.isValid());
      CHECK(tree.size() == 0);
      CHECK(tree.getRoot() == nullptr);
    }

    TEST(Persistance) {
      AvlTree<AvlNumericKey<i64>, TestClass> tree;

      struct Item {
        Item() :
            data(0) {}
        bool presents = false;
        TestClass data;
      };

      Item buff[size];

      for (auto i = 0; i < size; i++) {
        buff[i].data.setVal(i);
      }

      // random load
      ui64 loadSize = 0;
      while (loadSize < size / 2) {
        auto idx = ui64(randomFloat() * (size - 1));
        assert(idx < size);
        if (!buff[idx].presents) {
          tree.insert((i64) buff[idx].data.getVal(), buff[idx].data);
          loadSize++;
          buff[idx].presents = true;

          CHECK(tree.isValid());
          CHECK(tree.size() == loadSize);
        }
      }

      for (auto& item : buff) {
        if (item.presents) continue;
        tree.insert((i64) item.data.getVal(), item.data);
        loadSize++;
        item.presents = true;

        CHECK(tree.isValid());
        CHECK(tree.size() == loadSize);
      }

      CHECK(tree.size() == size);
      CHECK(tree.maxNode(tree.getRoot())->data.getVal() == size - 1);
      CHECK(tree.minNode(tree.getRoot())->data.getVal() == 0);

      // find
      for (auto item : buff) {
        auto node = tree.find((i64) item.data.getVal());
        CHECK(node);
        if (!node) continue;
        CHECK(node->data.getVal() == item.data.getVal());
      }

      CHECK(!tree.find(size + 1));
      CHECK(!tree.find(-1));

      // random unload
      ui64 unloadSize = 0;
      while (unloadSize < size / 2) {
        auto idx = ui64(randomFloat() * (size - 1));
        if (buff[idx].presents) {

          tree.remove((i64) buff[idx].data.getVal());

          unloadSize++;
          buff[idx].presents = false;

          // find
          for (auto item : buff) {
            if (!item.presents) continue;
            auto node = tree.find((i64) item.data.getVal());
            CHECK(node);
            if (!node) continue;
            CHECK(node->data.getVal() == item.data.getVal());
          }

          CHECK(tree.isValid());
          CHECK(tree.size() == size - unloadSize);
        }
      }

      for (auto& item : buff) {
        if (item.presents) {
          tree.remove((i64) item.data.getVal());
          unloadSize++;
          item.presents = false;

          CHECK(tree.isValid());
          CHECK(tree.size() == size - unloadSize);
        }
      }

      CHECK(tree.size() == 0);
      CHECK(tree.getRoot() == nullptr);
      CHECK(tree.maxNode(tree.getRoot()) == nullptr);
      CHECK(tree.minNode(tree.getRoot()) == nullptr);
    }
}