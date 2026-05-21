#ifndef BSTREE_H
#define BSTREE_H
#include <utility>

namespace alekseev {
  template< class Key, class Value >
  struct BSTree_node {
    std::pair< Key, Value > data;
    BSTree_node * left, * right, * parent;
  };

  template< class Key, class Value, class Compare >
  struct BSTree {
    explicit BSTree(Compare comp);
    ~BSTree();
    BSTree(BSTree & rhs);
    BSTree & operator=(BSTree & rhs);
    BSTree(BSTree && rhs);
    BSTree & operator=(BSTree && rhs);

    void clear();

    private:
      BSTree_node< Key, Value > * root_;
      Compare comp_;
      BSTree_node< Key, Value > * fake_leaf_;
  };

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(Compare comp):
    root_(nullptr),
    comp_(comp)
  {
    fake_leaf_ = static_cast< BSTree_node< Key, Value > * >(::operator new(
        sizeof(BSTree_node< Key, Value >)));
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::~BSTree()
  {
    clear();
    ::operator delete(fake_leaf_);
  }
}

#endif
