#ifndef BSTREE_H
#define BSTREE_H

#include <utility>
#include <stdexcept>

namespace alekseev {
  template< class Key, class Value >
  struct BSTree_node;
  template< class Key, class Value, class Compare >
  struct BSTree;
  template< class Key, class Value, class Compare >
  struct BSTIterator;
  template< class Key, class Value, class Compare >
  struct BSTConstIterator;

  template< class Key, class Value >
  struct BSTree_node {
    Key key;
    Value value;
    BSTree_node * left, * right, * parent;
  };

  template< class Key, class Value >
  void clear(BSTree_node< Key, Value > * root, BSTree_node< Key, Value > * fake_leaf);
  template< class Key, class Value >
  void swap_ptrs(BSTree_node< Key, Value > & a, BSTree_node< Key, Value > & b) noexcept;
  template< class Key, class Value >
  void swap_data(BSTree_node< Key, Value > & a, BSTree_node< Key, Value > & b);

  template< class Key, class Value, class Compare >
  struct BSTIterator {
    explicit BSTIterator(BSTree< Key, Value, Compare > * tree);
    ~BSTIterator() = default;
    BSTIterator(BSTIterator & rhs);
    BSTIterator & operator=(BSTIterator & rhs);
    BSTIterator(BSTIterator && rhs);
    BSTIterator & operator=(BSTIterator && rhs);

    private:
      BSTree< Key, Value, Compare > & tree_;
  };

  template< class Key, class Value, class Compare >
  struct BSTree {
    explicit BSTree(Compare comp);
    ~BSTree();
    BSTree(BSTree & rhs);
    BSTree & operator=(BSTree & rhs);
    BSTree(BSTree && rhs) noexcept;
    BSTree & operator=(BSTree && rhs) noexcept;

    using BST_n = BSTree_node< Key, Value >;

    void clear();
    void swap(BSTree & rhs) noexcept;
    void push(const Key & key, const Value & value);
    Value & at(const Key & key);
    const Value & at(const Key & key) const;
    void remove(const Key & key);

    private:
      BST_n * root_;
      Compare comp_;
      BST_n * fake_leaf_;
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

  template< class Key, class Value >
  void swap_ptrs(BSTree_node< Key, Value > & a, BSTree_node< Key, Value > & b) noexcept
  {
    std::swap(a.left, b.left);
    std::swap(a.right, b.right);
    std::swap(a.parent, b.parent);
  }

  template< class Key, class Value >
  void swap_data(BSTree_node< Key, Value > & a, BSTree_node< Key, Value > & b)
  {
    std::swap(a.key, b.key);
    std::swap(a.value, b.value);
  }

  template< class Key, class Value, class Compare >
  BSTIterator< Key, Value, Compare >::BSTIterator(BSTree< Key, Value, Compare > * tree):
    tree_(tree)
  {
  }

  template< class Key, class Value, class Compare >
  BSTIterator< Key, Value, Compare >::BSTIterator(BSTIterator & rhs):
    tree_(rhs.tree_)
  {
  }

  template< class Key, class Value, class Compare >
  BSTIterator< Key, Value, Compare > & BSTIterator< Key, Value, Compare >::operator=(
      BSTIterator & rhs)
  {
    tree_ = rhs.tree_;
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(Compare comp):
    comp_(comp)
  {
    fake_leaf_ = static_cast< BST_n * >(::operator new(sizeof(BST_n)));
    root_ = fake_leaf_;
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

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::push(const Key & key, const Value & value)
  {
    if (root_ == fake_leaf_) {
      root_ = new BST_n{key, value, fake_leaf_, fake_leaf_, nullptr};
    }
    BST_n * current = root_;
    while (current != fake_leaf_) {
      if (comp_(key, current->key)) {
        current = current->left;
      } else if (comp_(current->key, key)) {
        current = current->right;
      } else {
        current->value = value;
        return;
      }
    }
    current = current->parent;
    auto * new_node = new BST_n{key, value, fake_leaf_, fake_leaf_, current};
    if (comp_(key, current->key)) {
      current->left = new_node;
    } else {
      current->right = new_node;
    }
  }

  template< class Key, class Value, class Compare >
  Value & BSTree< Key, Value, Compare >::at(const Key & key)
  {
    return const_cast< Value & >(static_cast< const BSTree >(*this).at(key));
  }

  template< class Key, class Value, class Compare >
  const Value & BSTree< Key, Value, Compare >::at(const Key & key) const
  {
    BST_n * current = root_;
    while (current != fake_leaf_) {
      if (comp_(key, current->key)) {
        current = current->left;
      } else if (comp_(current->key, key)) {
        current = current->right;
      } else {
        return current->value;
      }
    }
    throw std::out_of_range("Key not found");
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::remove(const Key & key)
  {
    BST_n * current = root_;
    while (current != fake_leaf_) {
      if (comp_(key, current->key)) {
        current = current->left;
      } else if (comp_(current->key, key)) {
        current = current->right;
      } else {
        break;
      }
    }
    if (current == fake_leaf_) {
      return;
    }
    BST_n * found = current;
    if (current->right != fake_leaf_) {
      current = current->right;
      while (current->left != fake_leaf_) {
        current = current->left;
      }
      swap_ptrs(current, found);
      if (found->parent != nullptr) {
        found->parent->left = found->right;
      }
      found->right->parent = found->parent;
      delete found;
    } else {
      if (current->parent != nullptr) {
        if (current->parent->left == current) {
          current->parent->left = current->left;
        } else {
          current->parent->right = current->left;
        }
      }
      found->left->parent = current->parent;
      delete current;
    }
  }
}

#endif
