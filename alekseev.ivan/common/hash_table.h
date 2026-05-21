#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "List.h"
#include "vector.h"
#include <cstddef>
#include <stdexcept>

namespace alekseev {
  template< class Key, class Value, class Hash, class Equal >
  struct HashTable {
    using Pair = std::pair< Key, Value >;
    ~HashTable();
    HashTable(const HashTable & rhs);
    HashTable & operator=(const HashTable & rhs);
    HashTable(HashTable && rhs) noexcept;
    HashTable & operator=(HashTable && rhs) noexcept;

    HashTable(Hash hasher, Equal is_equal, size_t capacity);

    void swap(HashTable & rhs) noexcept;
    void clear();
    void insert(const Key & key, const Value & value);
    void remove(const Key & key);
    Value & at(const Key & key);
    const Value & at(const Key & key) const;
    bool contains(const Key & key) const;
    Vector< Key > keys() const;
    double load_factor() const;
    void rehash(size_t new_capacity);
    size_t capacity() const;
    size_t size() const;

    private:
      size_t capacity_, size_;
      List< Pair > ** slots_;
      Hash hasher_;
      Equal is_equal_;

      List< Pair > * find_previous_node(const Key & key);
      const List< Pair > * find_previous_node(const Key & key) const;
  };

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::~HashTable()
  {
    clear();
    delete[] slots_;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable(const HashTable & rhs):
    capacity_(rhs.capacity_),
    size_(rhs.size_),
    hasher_(rhs.hasher_),
    is_equal_(rhs.is_equal_)

  {
    slots_ = new List< Pair > *[capacity_]{nullptr};
    for (size_t i = 0; i < rhs.capacity_; ++i) {
      if (rhs.slots_[i]) {
        slots_[i] = deep_copy(rhs.slots_[i]);
      }
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal > & HashTable< Key, Value, Hash, Equal >::operator=(
      const HashTable & rhs)
  {
    HashTable< Key, Value, Hash, Equal > temp(rhs);
    swap(temp);
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable(HashTable && rhs) noexcept:
    capacity_(rhs.capacity_),
    size_(rhs.size_),
    slots_(rhs.slots_),
    hasher_(rhs.hasher_),
    is_equal_(rhs.is_equal_)
  {
    rhs.size_ = 0;
    rhs.capacity_ = 0;
    rhs.slots_ = nullptr;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal > & HashTable< Key, Value, Hash, Equal >::operator=(
      HashTable && rhs) noexcept
  {
    swap(rhs);
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable(Hash hasher, Equal is_equal, size_t capacity):
    capacity_(capacity),
    size_(0),
    slots_(new List< Pair > *[capacity]{nullptr}),
    hasher_(hasher),
    is_equal_(is_equal)
  {
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::swap(HashTable & rhs) noexcept
  {
    std::swap(capacity_, rhs.capacity_);
    std::swap(size_, rhs.size_);
    std::swap(is_equal_, rhs.is_equal_);
    std::swap(hasher_, rhs.hasher_);
    std::swap(slots_, rhs.slots_);
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::clear()
  {
    for (size_t i = 0; i < capacity_; ++i) {
      if (slots_[i]) {
        alekseev::clear(slots_[i]->next, slots_[i]);
        rmfake(slots_[i]);
        slots_[i] = nullptr;
      }
    }
    size_ = 0;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::insert(const Key & key, const Value & value)
  {
    size_t index = hasher_(key) % capacity_;
    List< Pair > * tail = nullptr;
    if (slots_[index]) {
      List< Pair > * fake = slots_[index];
      List< Pair > * current = fake;
      while (current->next != fake) {
        current = current->next;
        if (is_equal_(current->data.first, key)) {
          current->data.second = value;
          return;
        }
      }
      tail = current;
    } else {
      slots_[index] = fake< Pair >();
      tail = slots_[index];
    }
    try {
      insert_after(tail, std::pair< Key, Value >(key, value));
      ++size_;
    } catch (...) {
      if (tail->next == tail) {
        rmfake(tail);
        slots_[index] = nullptr;
      }
      throw;
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::remove(const Key & key)
  {
    List< Pair > * pre_node = find_previous_node(key);
    if (!pre_node) {
      return;
    }
    erase_after(pre_node);
    --size_;
    if (pre_node->next == pre_node) {
      rmfake(pre_node);
      size_t index = hasher_(key) % capacity_;
      slots_[index] = nullptr;
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  Value & HashTable< Key, Value, Hash, Equal >::at(const Key & key)
  {
    return const_cast< Value & >(static_cast< const HashTable * >(this)->at(key));
  }

  template< class Key, class Value, class Hash, class Equal >
  const Value & HashTable< Key, Value, Hash, Equal >::at(const Key & key) const
  {
    const List< Pair > * found = find_previous_node(key);
    if (found) {
      return found->next->data.second;
    }
    throw std::out_of_range("Key not found");
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::contains(const Key & key) const
  {
    return (*static_cast< const HashTable * >(this)).find_previous_node(key) != nullptr;
  }

  template< class Key, class Value, class Hash, class Equal >
  Vector<Key> HashTable<Key, Value, Hash, Equal>::keys() const
  {
    Vector< Key > res;
    for (size_t i = 0; i < capacity_; ++i) {
      if (slots_[i]) {
        List< Pair > * current = slots_[i]->next;
        while (current != slots_[i]) {
          res.pushBack(current->data.first);
          current = current->next;
        }
      }
    }
    return res;
  }

  template< class Key, class Value, class Hash, class Equal >
  double HashTable< Key, Value, Hash, Equal >::load_factor() const
  {
    if (size() == 0) {
      return 0;
    }
    double s = size_;
    return s / capacity_;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::rehash(size_t new_capacity)
  {
    HashTable temp(hasher_, is_equal_, new_capacity);
    for (size_t i = 0; i < capacity_; ++i) {
      if (slots_[i]) {
        List< Pair > * fake = slots_[i];
        List< Pair > * current = fake->next;
        while (current != fake) {
          temp.insert(current->data.first, current->data.second);
          current = current->next;
        }
      }
    }
    swap(temp);
  }

  template< class Key, class Value, class Hash, class Equal >
  size_t HashTable< Key, Value, Hash, Equal >::capacity() const
  {
    return capacity_;
  }

  template< class Key, class Value, class Hash, class Equal >
  size_t HashTable< Key, Value, Hash, Equal >::size() const
  {
    return size_;
  }

  template< class Key, class Value, class Hash, class Equal >
  List< std::pair< Key, Value > > * HashTable< Key, Value, Hash, Equal >::find_previous_node(
      const Key & key)
  {
    return const_cast< List< Pair > * >(static_cast< const HashTable * >(this)->
      find_previous_node(key));
  }

  template< class Key, class Value, class Hash, class Equal >
  const List< std::pair< Key, Value > > * HashTable< Key, Value, Hash, Equal >::find_previous_node(
      const Key & key) const
  {
    size_t index = hasher_(key) % capacity_;
    if (!slots_[index]) {
      return nullptr;
    }
    List< Pair > * fake = slots_[index];
    List< Pair > * current = fake;
    while (current->next != fake) {
      if (is_equal_(current->next->data.first, key)) {
        return current;
      }
      current = current->next;
    }
    return nullptr;
  }
}
#endif
