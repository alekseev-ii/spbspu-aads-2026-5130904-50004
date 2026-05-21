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
    BSTree();
    ~BSTree();
    BSTree(BSTree & rhs);
    BSTree & operator=(BSTree & rhs);
    BSTree(BSTree && rhs);
    BSTree & operator=(BSTree && rhs);

    private:
      BSTree_node< Key, Value > * root;
      Compare comp;
      BSTree_node< Key, Value > * fake_leaf;
  };
}

#endif
