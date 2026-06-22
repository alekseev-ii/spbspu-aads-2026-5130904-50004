#ifndef BSTREE_H
#define BSTREE_H

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
  struct BSTree_node
  {
    Key key;
    Value value;
    BSTree_node * left, * right, * parent;
  };

  template< class Key, class Value >
  void clear(BSTree_node< Key, Value > * root, BSTree_node< Key, Value > * fake_leaf) noexcept;

  template< class Key, class Value >
  BSTree_node< Key, Value > * copy(BSTree_node< Key, Value > * old_parent,
      BSTree_node< Key, Value > * old_fake_leaf,
      BSTree_node< Key, Value > * new_parent,
      BSTree_node< Key, Value > * new_fake_leaf);

  template< class Key, class Value >
  void swap_ptrs(BSTree_node< Key, Value > * a,
      BSTree_node< Key, Value > * b,
      BSTree_node< Key, Value > * fake_leaf) noexcept;

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
  size_t height(BSTree_node< Key, Value > * root, BSTree_node< Key, Value > * fake_leaf);

  template< class Key, class Value >
  struct BSTIterator
  {
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

    BSTree_node< Key, Value > * current_;
    BSTree_node< Key, Value > * fake_leaf_;
  };

  template< class Key, class Value >
  struct BSTConstIterator
  {
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

    BSTree_node< Key, Value > * current_;
    BSTree_node< Key, Value > * fake_leaf_;
  };

  template< class Key, class Value, class IterType >
  IterType begin(BSTree_node< Key, Value > * root,
      BSTree_node< Key, Value > * fake_leaf);
  template< class Key, class Value, class IterType >
  IterType end(BSTree_node< Key, Value > * root,
      BSTree_node< Key, Value > * fake_leaf);
  template< class Key, class Value, class Compare, class IterType >
  IterType begin(const BSTree< Key, Value, Compare > & tree);
  template< class Key, class Value, class Compare, class IterType >
  IterType end(const BSTree< Key, Value, Compare > & tree);

  template< class Key, class Value, class Compare >
  struct BSTree
  {
    explicit BSTree(Compare comp);
    ~BSTree();
    BSTree(const BSTree & rhs);
    BSTree & operator=(const BSTree & rhs);
    BSTree(BSTree && rhs) noexcept;
    BSTree & operator=(BSTree && rhs) noexcept;

    using BST_n = BSTree_node< Key, Value >;

    void clear() noexcept;
    void swap(BSTree & rhs) noexcept;
    void push(const Key & key, const Value & value);
    Value & at(const Key & key);
    const Value & at(const Key & key) const;
    void remove(const Key & key);
    bool contains(const Key & key) const;
    bool empty() const;

    friend struct BSTConstIterator< Key, Value >;
    friend struct BSTIterator< Key, Value >;

    using const_iter_t = BSTConstIterator< Key, Value >;
    using iter_t = BSTIterator< Key, Value >;
    const_iter_t cbegin() const;
    const_iter_t cend() const;
    iter_t begin();
    iter_t end();

    template< class BSTIterator >
    BSTIterator rotateLeft(BSTIterator it);
    template< class BSTIterator >
    BSTIterator rotateRight(BSTIterator it);
    template< class BSTIterator >
    BSTIterator rotateLargeLeft(BSTIterator it);
    template< class BSTIterator >
    BSTIterator rotateLargeRight(BSTIterator it);

    size_t height(const_iter_t it) const;
    size_t height(iter_t it) const;
    size_t height() const;

    private:
      BST_n * root_;
      Compare comp_;
      BST_n * fake_leaf_;
  };

  template< class Key, class Value >
  void clear(BSTree_node< Key, Value > * root, BSTree_node< Key, Value > * fake_leaf) noexcept
  {
    if (root == fake_leaf) {
      return;
    }
    clear(root->left, fake_leaf);
    clear(root->right, fake_leaf);
    delete root;
  }

  template< class Key, class Value >
  BSTree_node< Key, Value > * copy(BSTree_node< Key, Value > * old_parent,
      BSTree_node< Key, Value > * old_fake_leaf,
      BSTree_node< Key, Value > * new_parent,
      BSTree_node< Key, Value > * new_fake_leaf)
  {
    if (old_parent == old_fake_leaf) {
      return new_fake_leaf;
    }
    auto * new_node = new BSTree_node< Key, Value >;
    try {
      new_node->key = old_parent->key;
      new_node->value = old_parent->value;
    } catch (...) {
      delete new_node;
      throw;
    }
    new_node->left = new_fake_leaf;
    new_node->right = new_fake_leaf;
    new_node->parent = new_parent;

    new_node->left = copy(old_parent->left, old_fake_leaf, new_node, new_fake_leaf);
    new_node->right = copy(old_parent->right, old_fake_leaf, new_node, new_fake_leaf);
    return new_node;
  }

  template< class Key, class Value >
  void swap_ptrs(BSTree_node< Key, Value > * a, BSTree_node< Key, Value > * b,
      BSTree_node< Key, Value > * fake_leaf) noexcept
  {
    if (a->left != fake_leaf) {
      a->left->parent = b;
    }
    if (a->right != fake_leaf) {
      a->right->parent = b;
    }
    if (b->left != fake_leaf) {
      b->left->parent = a;
    }
    if (b->right != fake_leaf) {
      b->right->parent = a;
    }

    if (a->parent != nullptr) {
      if (a->parent->left == a) {
        a->parent->left = b;
      } else {
        a->parent->right = b;
      }
    }
    if (b->parent != nullptr) {
      if (b->parent->left == b) {
        b->parent->left = a;
      } else {
        b->parent->right = a;
      }
    }

    std::swap(a->left, b->left);
    std::swap(a->right, b->right);
    std::swap(a->parent, b->parent);
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
      return fake_leaf;
    }
    if (current->right != fake_leaf) {
      return fall_left(current->right, fake_leaf);
    }
    if (current->parent == nullptr || current->parent == fake_leaf) {
      return fake_leaf;
    }
    while (current->parent->left != current) {
      current = current->parent;
      if (current->parent == nullptr) {
        return fake_leaf;
      }
    }
    return current->parent;
  }

  template< class Key, class Value >
  BSTree_node< Key, Value > * prev(BSTree_node< Key, Value > * current,
      BSTree_node< Key, Value > * fake_leaf)
  {
    if (current == fake_leaf) {
      return fake_leaf;
    }
    if (current->left != fake_leaf) {
      return fall_right(current->left, fake_leaf);
    }
    if (current->parent == nullptr || current->parent == fake_leaf) {
      return fake_leaf;
    }
    while (current->parent->right != current) {
      current = current->parent;
      if (current->parent == nullptr) {
        return fake_leaf;
      }
    }
    return current->parent;
  }

  template< class Key, class Value >
  size_t height(BSTree_node< Key, Value > * root, BSTree_node< Key, Value > * fake_leaf)
  {
    if (root == fake_leaf) {
      return 0;
    }
    size_t left = height(root->left, fake_leaf);
    size_t right = height(root->right, fake_leaf);
    return std::max(left, right) + 1ull;
  }

  template< class Key, class Value >
  BSTIterator< Key, Value >::BSTIterator(BSTree_node< Key, Value > * current,
      BSTree_node< Key, Value > * fake_leaf):
    current_(current),
    fake_leaf_(fake_leaf)
  { }

  template< class Key, class Value >
  BSTIterator< Key, Value >::BSTIterator(BSTIterator && rhs) noexcept:
    current_(rhs.current_),
    fake_leaf_(rhs.fake_leaf_)
  { }

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
  { }

  template< class Key, class Value >
  BSTConstIterator< Key, Value >::BSTConstIterator(BSTConstIterator && rhs) noexcept:
    current_(rhs.current_),
    fake_leaf_(rhs.fake_leaf_)
  { }

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

  template< class Key, class Value, class IterType >
  IterType begin(BSTree_node< Key, Value > * root,
      BSTree_node< Key, Value > * fake_leaf)
  {
    return IterType(fall_left(root, fake_leaf), fake_leaf);
  }

  template< class Key, class Value, class IterType >
  IterType end(BSTree_node< Key, Value > *,
      BSTree_node< Key, Value > * fake_leaf)
  {
    return IterType(fake_leaf, fake_leaf);
  }

  template< class Key, class Value, class Compare, class IterType >
  IterType begin(const BSTree< Key, Value, Compare > & tree)
  {
    return begin< Key, Value, IterType >(tree.root_, tree.fake_leaf_);
  }

  template< class Key, class Value, class Compare, class IterType >
  IterType end(const BSTree< Key, Value, Compare > & tree)
  {
    return end< Key, Value, IterType >(tree.root_, tree.fake_leaf_);
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(Compare comp):
    comp_(comp)
  {
    fake_leaf_ = static_cast< BST_n * >(::operator new(sizeof(BST_n)));
    fake_leaf_->parent = fake_leaf_;
    fake_leaf_->left = fake_leaf_;
    fake_leaf_->right = fake_leaf_;
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
    root_(nullptr),
    comp_(rhs.comp_),
    fake_leaf_(nullptr)
  {
    fake_leaf_ = static_cast< BST_n * >(::operator new(sizeof(BST_n)));
    fake_leaf_->parent = fake_leaf_;
    fake_leaf_->left = fake_leaf_;
    fake_leaf_->right = fake_leaf_;
    root_ = fake_leaf_;

    if (rhs.root_ == rhs.fake_leaf_) {
      return;
    }
    try {
      root_ = alekseev::copy< Key, Value >(rhs.root_, rhs.fake_leaf_, nullptr, fake_leaf_);
    } catch (...) {
      clear();
      ::operator delete(fake_leaf_);
      throw;
    }
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > & BSTree< Key, Value, Compare >::operator=(const BSTree & rhs)
  {
    if (this == std::addressof(rhs)) {
      return *this;
    }
    BSTree tmp(rhs);
    swap(tmp);
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
  void BSTree< Key, Value, Compare >::clear() noexcept
  {
    alekseev::clear(root_, fake_leaf_);
    root_ = fake_leaf_;
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
      return;
    }
    BST_n * current = root_;
    BST_n * parent = nullptr;
    while (current != fake_leaf_) {
      parent = current;
      if (comp_(key, current->key)) {
        current = current->left;
      } else if (comp_(current->key, key)) {
        current = current->right;
      } else {
        current->value = value;
        return;
      }
    }
    auto * new_node = new BST_n{key, value, fake_leaf_, fake_leaf_, parent};
    if (comp_(key, parent->key)) {
      parent->left = new_node;
    } else {
      parent->right = new_node;
    }
  }

  template< class Key, class Value, class Compare >
  Value & BSTree< Key, Value, Compare >::at(const Key & key)
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
    BST_n * to_remove = root_;
    while (to_remove != fake_leaf_) {
      if (comp_(key, to_remove->key)) {
        to_remove = to_remove->left;
      } else if (comp_(to_remove->key, key)) {
        to_remove = to_remove->right;
      } else {
        break;
      }
    }
    if (to_remove == fake_leaf_) {
      return;
    }
    if (to_remove->right == fake_leaf_) {
      BST_n * child = to_remove->left;
      if (to_remove->parent == nullptr) {
        root_ = child;
      } else {
        if (to_remove->parent->left == to_remove) {
          to_remove->parent->left = child;
        } else {
          to_remove->parent->right = child;
        }
      }
      if (child != fake_leaf_) {
        child->parent = to_remove->parent;
      }
      delete to_remove;
      return;
    }

    if (to_remove->left != fake_leaf_) {
      BST_n * successor = fall_left(to_remove->right, fake_leaf_);
      swap_ptrs(to_remove, successor, fake_leaf_);
      if (to_remove->parent == nullptr) {
        root_ = to_remove;
      }
      if (successor->parent == nullptr) {
        root_ = successor;
      }
    }

    BST_n * child = to_remove->right;
    if (to_remove->parent == nullptr) {
      root_ = child;
    } else {
      if (to_remove->parent->left == to_remove) {
        to_remove->parent->left = child;
      } else {
        to_remove->parent->right = child;
      }
    }
    if (child != fake_leaf_) {
      child->parent = to_remove->parent;
    }
    delete to_remove;
  }

  template< class Key, class Value, class Compare >
  bool BSTree< Key, Value, Compare >::contains(const Key & key) const
  {
    BST_n * current = root_;
    while (current != fake_leaf_) {
      if (comp_(key, current->key)) {
        current = current->left;
      } else if (comp_(current->key, key)) {
        current = current->right;
      } else {
        return true;
      }
    }
    return false;
  }

  template< class Key, class Value, class Compare >
  bool BSTree< Key, Value, Compare >::empty() const
  {
    return root_ == fake_leaf_;
  }

  template< class Key, class Value, class Compare >
  BSTConstIterator< Key, Value > BSTree< Key, Value, Compare >::cbegin() const
  {
    return alekseev::begin< Key, Value, const_iter_t >(root_, fake_leaf_);
  }

  template< class Key, class Value, class Compare >
  BSTConstIterator< Key, Value > BSTree< Key, Value, Compare >::cend() const
  {
    return alekseev::end< Key, Value, const_iter_t >(root_, fake_leaf_);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iter_t BSTree< Key, Value, Compare >::begin()
  {
    return alekseev::begin< Key, Value, iter_t >(root_, fake_leaf_);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iter_t BSTree< Key, Value, Compare >::end()
  {
    return alekseev::end< Key, Value, iter_t >(root_, fake_leaf_);
  }

  template< class Key, class Value, class Compare >
  template< class BSTIterator >
  BSTIterator BSTree< Key, Value, Compare >::rotateLeft(
      BSTIterator it)
  {
    BST_n * current = it.current_;
    if (current == fake_leaf_ || current->parent == nullptr) {
      throw std::invalid_argument("it is a root or fake_leaf!");
    }
    BST_n * parent = current->parent;
    BST_n * left = current->left;

    parent->right = left;
    left->parent = parent;

    current->parent = parent->parent;
    if (current->parent != nullptr) {
      if (current->parent->left == parent) {
        current->parent->left = current;
      } else {
        current->parent->right = current;
      }
    } else {
      root_ = current;
    }

    current->left = parent;
    parent->parent = current;
    return it;
  }

  template< class Key, class Value, class Compare >
  template< class BSTIterator >
  BSTIterator BSTree< Key, Value, Compare >::rotateRight(
      BSTIterator it)
  {
    BST_n * current = it.current_;
    if (current == fake_leaf_ || current->parent == nullptr) {
      throw std::invalid_argument("it is a root!");
    }
    BST_n * parent = current->parent;
    BST_n * right = current->right;

    parent->left = right;
    right->parent = parent;

    current->parent = parent->parent;
    if (current->parent != nullptr) {
      if (current->parent->left == parent) {
        current->parent->left = current;
      } else {
        current->parent->right = current;
      }
    } else {
      root_ = current;
    }

    current->right = parent;
    parent->parent = current;
    return it;
  }

  template< class Key, class Value, class Compare >
  template< class BSTIterator >
  BSTIterator BSTree< Key, Value, Compare >::
  rotateLargeLeft(BSTIterator it)
  {
    return rotateLeft(rotateRight(it));
  }

  template< class Key, class Value, class Compare >
  template< class BSTIterator >
  BSTIterator BSTree< Key, Value, Compare >::
  rotateLargeRight(BSTIterator it)
  {
    return rotateRight(rotateLeft(it));
  }

  template< class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::height(const_iter_t it) const
  {
    return alekseev::height(it.current_, fake_leaf_);
  }

  template< class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::height(iter_t it) const
  {
    return alekseev::height(it.current_, fake_leaf_);
  }

  template< class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::height() const
  {
    return alekseev::height(root_, fake_leaf_);
  }
}

#endif
