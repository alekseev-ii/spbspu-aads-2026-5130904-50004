#ifndef BSTREE_H
#define BSTREE_H

#include <utility>
#include <stdexcept>

namespace alekseev {
  template< class Key, class Value >
  struct BSTree_node;
  template< class Key, class Value, class Compare >
  struct BSTree;
  template< class Key, class Value >
  struct BSTIterator;
  template< class Key, class Value >
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
  BSTree_node< Key, Value > * copy(BSTree_node< Key, Value > * root,
      BSTree_node< Key, Value > * fake_leaf, BSTree_node< Key, Value > * new_parent,
      BSTree_node< Key, Value > * new_fake_leaf);
  template< class Key, class Value >
  void swap_ptrs(BSTree_node< Key, Value > & a, BSTree_node< Key, Value > & b) noexcept;
  template< class Key, class Value >
  void swap_data(BSTree_node< Key, Value > & a, BSTree_node< Key, Value > & b);
  template< class Key, class Value >
  BSTree_node< Key, Value > * fall_left(BSTree_node< Key, Value > * current,
      BSTree_node< Key, Value > * fake_leaf);
  template< class Key, class Value >
  BSTree_node< Key, Value > * fall_right(BSTree_node< Key, Value > * current,
      BSTree_node< Key, Value > * fake_leaf);
  template< class Key, class Value >
  BSTree_node< Key, Value > * next(BSTree_node< Key, Value > * current,
      BSTree_node< Key, Value > * fake_leaf);
  template< class Key, class Value >
  BSTree_node< Key, Value > * prev(BSTree_node< Key, Value > * current,
      BSTree_node< Key, Value > * fake_leaf);

  template< class Key, class Value >
  struct BSTIterator {
    explicit BSTIterator(BSTree_node< Key, Value > * current,
        BSTree_node< Key, Value > * fake_leaf);
    ~BSTIterator() = default;
    BSTIterator(const BSTIterator & rhs) = default;
    BSTIterator & operator=(const BSTIterator & rhs) = default;
    BSTIterator(BSTIterator && rhs) noexcept;
    BSTIterator & operator=(BSTIterator && rhs) noexcept;

    BSTIterator & operator++();
    BSTIterator operator++(int);
    BSTIterator & operator--();
    BSTIterator operator--(int);

    template< class OtherBSTIterator >
    bool operator==(OtherBSTIterator rhs);
    template< class OtherBSTIterator >
    bool operator!=(OtherBSTIterator rhs);

    Value & operator*() const;
    Value * operator->() const;

    private:
      BSTree_node< Key, Value > * current_;
      BSTree_node< Key, Value > * fake_leaf_;
  };

  template< class Key, class Value >
  struct BSTConstIterator {
    explicit BSTConstIterator(BSTree_node< Key, Value > * current,
        BSTree_node< Key, Value > * fake_leaf);
    ~BSTConstIterator() = default;
    BSTConstIterator(const BSTConstIterator & rhs) = default;
    BSTConstIterator & operator=(const BSTConstIterator & rhs) = default;
    BSTConstIterator(BSTConstIterator && rhs) noexcept;
    BSTConstIterator & operator=(BSTConstIterator && rhs) noexcept;

    BSTConstIterator & operator++();
    BSTConstIterator operator++(int);
    BSTConstIterator & operator--();
    BSTConstIterator operator--(int);

    template< class OtherBSTIterator >
    bool operator==(OtherBSTIterator rhs);
    template< class OtherBSTIterator >
    bool operator!=(OtherBSTIterator rhs);

    const Value & operator*() const;
    const Value * operator->() const;

    private:
      BSTree_node< Key, Value > * current_;
      BSTree_node< Key, Value > * fake_leaf_;
  };

  template< class Key, class Value, class Compare >
  struct BSTree {
    explicit BSTree(Compare comp);
    ~BSTree();
    BSTree(const BSTree & rhs);
    BSTree & operator=(const BSTree & rhs);
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
  BSTree_node< Key, Value > * copy(BSTree_node< Key, Value > * root,
      BSTree_node< Key, Value > * fake_leaf, BSTree_node< Key, Value > * new_parent,
      BSTree_node< Key, Value > * new_fake_leaf)
  {
    if (root == fake_leaf) {
      return new_fake_leaf;
    }
    auto * new_node = new BSTree_node< Key, Value >;
    new_node->key = root->key;
    new_node->value = root->value;
    new_node->left = copy(root->left, new_node, fake_leaf);
    new_node->right = copy(root->right, new_node, fake_leaf);
    new_node->parent = new_parent;
    return new_node;
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

  template< class Key, class Value >
  BSTree_node< Key, Value > * fall_left(BSTree_node< Key, Value > * current,
      BSTree_node< Key, Value > * fake_leaf)
  {
    if (current == fake_leaf) {
      return fake_leaf;
    }
    while (current->left != fake_leaf) {
      current = current->left;
    }
    return current;
  }

  template< class Key, class Value >
  BSTree_node< Key, Value > * fall_right(BSTree_node< Key, Value > * current,
      BSTree_node< Key, Value > * fake_leaf)
  {
    if (current == fake_leaf) {
      return fake_leaf;
    }
    while (current->right != fake_leaf) {
      current = current->right;
    }
    return current;
  }

  template< class Key, class Value >
  BSTree_node< Key, Value > * next(BSTree_node< Key, Value > * current,
      BSTree_node< Key, Value > * fake_leaf)
  {
    if (current == fake_leaf) {
      return nullptr;
    }
    if (current->right != fake_leaf) {
      return fall_left(current->right, fake_leaf);
    }
    if (current->parent == nullptr) {
      return nullptr;
    }
    while (current->parent->left != current) {
      current = current->parent;
      if (current->parent == nullptr) {
        return nullptr;
      }
    }
    return current->parent;
  }

  template< class Key, class Value >
  BSTree_node< Key, Value > * prev(BSTree_node< Key, Value > * current,
      BSTree_node< Key, Value > * fake_leaf)
  {
    if (current == fake_leaf) {
      return nullptr;
    }
    if (current->left != fake_leaf) {
      return fall_right(current->left, fake_leaf);
    }
    if (current->parent == nullptr) {
      return nullptr;
    }
    while (current->parent->right != current) {
      current = current->parent;
      if (current->parent == nullptr) {
        return nullptr;
      }
    }
    return current->parent;
  }

  template< class Key, class Value >
  BSTIterator< Key, Value >::BSTIterator(BSTree_node< Key, Value > * current,
      BSTree_node< Key, Value > * fake_leaf):
    current_(current),
    fake_leaf_(fake_leaf)
  {
  }

  template< class Key, class Value >
  BSTIterator< Key, Value >::BSTIterator(BSTIterator && rhs) noexcept:
    current_(rhs.current_),
    fake_leaf_(rhs.fake_leaf_)
  {
  }

  template< class Key, class Value >
  BSTIterator< Key, Value > & BSTIterator< Key, Value >::operator=(BSTIterator && rhs) noexcept
  {
    current_ = rhs.current_;
    fake_leaf_ = rhs.fake_leaf_;
    return *this;
  }

  template< class Key, class Value >
  BSTIterator< Key, Value > & BSTIterator< Key, Value >::operator++()
  {
    current_ = next(current_, fake_leaf_);
    return *this;
  }

  template< class Key, class Value >
  BSTIterator< Key, Value > BSTIterator< Key, Value >::operator++(int)
  {
    BSTIterator tmp = *this;
    ++(*this);
    return tmp;
  }

  template< class Key, class Value >
  BSTIterator< Key, Value > & BSTIterator< Key, Value >::operator--()
  {
    current_ = prev(current_, fake_leaf_);
    return *this;
  }

  template< class Key, class Value >
  BSTIterator< Key, Value > BSTIterator< Key, Value >::operator--(int)
  {
    BSTIterator tmp = *this;
    --(*this);
    return tmp;
  }

  template< class Key, class Value >
  template< class OtherBSTIterator >
  bool BSTIterator< Key, Value >::operator==(OtherBSTIterator rhs)
  {
    return current_ == rhs.current_;
  }

  template< class Key, class Value >
  template< class OtherBSTIterator >
  bool BSTIterator< Key, Value >::operator!=(OtherBSTIterator rhs)
  {
    return current_ != rhs.current_;
  }

  template< class Key, class Value >
  Value & BSTIterator< Key, Value >::operator*() const
  {
    return current_->value;
  }

  template< class Key, class Value >
  Value * BSTIterator< Key, Value >::operator->() const
  {
    return std::addressof(current_->value);
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value >::BSTConstIterator(BSTree_node< Key, Value > * current,
      BSTree_node< Key, Value > * fake_leaf):
    current_(current),
    fake_leaf_(fake_leaf)
  {
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value >::BSTConstIterator(BSTConstIterator && rhs) noexcept:
    current_(rhs.current_),
    fake_leaf_(rhs.fake_leaf_)
  {
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value > & BSTConstIterator< Key, Value >::operator=(
      BSTConstIterator && rhs) noexcept
  {
    current_ = rhs.current_;
    fake_leaf_ = rhs.fake_leaf_;
    return *this;
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value > & BSTConstIterator< Key, Value >::operator++()
  {
    current_ = next(current_, fake_leaf_);
    return *this;
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value > BSTConstIterator< Key, Value >::operator++(int)
  {
    BSTConstIterator tmp = *this;
    ++(*this);
    return tmp;
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value > & BSTConstIterator< Key, Value >::operator--()
  {
    current_ = prev(current_, fake_leaf_);
    return *this;
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value > BSTConstIterator< Key, Value >::operator--(int)
  {
    BSTConstIterator tmp = *this;
    --(*this);
    return tmp;
  }

  template< class Key, class Value >
  template< class OtherBSTIterator >
  bool BSTConstIterator< Key, Value >::operator==(OtherBSTIterator rhs)
  {
    return current_ == rhs.current_;
  }

  template< class Key, class Value >
  template< class OtherBSTIterator >
  bool BSTConstIterator< Key, Value >::operator!=(OtherBSTIterator rhs)
  {
    return current_ != rhs.current_;
  }

  template< class Key, class Value >
  const Value & BSTConstIterator< Key, Value >::operator*() const
  {
    return current_->value;
  }

  template< class Key, class Value >
  const Value * BSTConstIterator< Key, Value >::operator->() const
  {
    return std::addressof(current_->value);
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
  BSTree< Key, Value, Compare >::BSTree(const BSTree & rhs):
    comp_(rhs.comp_),
    fake_leaf_(static_cast< BST_n * >(::operator new(sizeof(BST_n))))
  {
    root_ = copy(rhs.root_, rhs.fake_leaf_, nullptr, fake_leaf_);
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > & BSTree< Key, Value, Compare >::operator=(const BSTree & rhs)
  {
    BST_n * temp_root = copy(rhs.root_, rhs.fake_leaf_, nullptr, fake_leaf_);
    alekseev::clear(root_, fake_leaf_);
    root_ = temp_root;
    return *this;
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
