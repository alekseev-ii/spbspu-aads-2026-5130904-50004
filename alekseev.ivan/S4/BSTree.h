#ifndef BSTREE_H
#define BSTREE_H
#include <utility>

namespace alekseev {
  template< class Key, class Value >
  struct BSTree_node {
    std::pair< Key, Value > data;
    BSTree_node * left, * right, * parent;
  };

  template< class Key, class Value >
  void clear(BSTree_node< Key, Value > * root, BSTree_node< Key, Value > * fake_leaf);

  template< class Key, class Value, class Compare >
  struct BSTree {
    explicit BSTree(Compare comp);
    ~BSTree();
    BSTree(BSTree & rhs);
    BSTree & operator=(BSTree & rhs);
    BSTree(BSTree && rhs) noexcept;
    BSTree & operator=(BSTree && rhs) noexcept;

    void clear();
    void swap(BSTree & rhs) noexcept;
    void push(const Key & key, const Value & value);
    Value get(const Key & key) const;
    void remove(const Key & key);

    private:
      BSTree_node< Key, Value > * root_;
      Compare comp_;
      BSTree_node< Key, Value > * fake_leaf_;
  };

  template< class Key, class Value >
  void clear(BSTree_node< Key, Value > * root, BSTree_node< Key, Value > * fake_leaf)
  {
    if (root == fake_leaf) {
      return;
    }
    clear(root->left, fake_leaf);
    clear(root->right, fake_leaf);
    delete root;
  }

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
    if (fake_leaf_ != nullptr) {
      ::operator delete(fake_leaf_);
    }
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(BSTree && rhs) noexcept:
    root_(rhs.root_),
    comp_(rhs.comp_),
    fake_leaf_(rhs.fake_leaf_)
  {
    rhs.root_ = nullptr;
    rhs.fake_leaf_ = nullptr;
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > & BSTree< Key, Value, Compare >::operator=(BSTree && rhs) noexcept
  {
    swap(rhs);
    return *this;
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::clear()
  {
    alekseev::clear(root_, fake_leaf_);
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::swap(BSTree & rhs) noexcept
  {
    std::swap(root_, rhs.root_);
    std::swap(comp_, rhs.comp_);
    std::swap(fake_leaf_, rhs.fake_leaf_);
  }
}

#endif
