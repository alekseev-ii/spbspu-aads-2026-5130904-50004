#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "List.h"
#include "vector.h"
#include <cstddef>
#include <stdexcept>

namespace alekseev {
  template< class Key, class Value, class Hash, class Equal >
  struct HashTable
  {
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
      List< Pair > * slots_;
      Hash hasher_;
      Equal is_equal_;

      std::pair< size_t, typename List< Pair >::LIter > find_previous_node(const Key & key);
      std::pair< size_t, typename List< Pair >::LCIter > find_previous_node(const Key & key) const;
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
    slots_ = new List< Pair >[capacity_]{List< Pair >()};
    for (size_t i = 0; i < rhs.capacity_; ++i) {
      if (!rhs.slots_[i].empty()) {
        slots_[i] = rhs.slots_[i];
      }
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal > & HashTable< Key, Value, Hash, Equal >::operator=(
      const HashTable & rhs)
  {
    if (this != std::addressof(rhs)) {
      HashTable temp(rhs);
      swap(temp);
    }
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
    if (this != std::addressof(rhs)) {
      swap(rhs);
    }
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable(Hash hasher, Equal is_equal, size_t capacity):
    capacity_(capacity),
    size_(0),
    slots_(new List< Pair >[capacity]{List< Pair >()}),
    hasher_(hasher),
    is_equal_(is_equal)
  { }

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
    if (slots_ == nullptr) {
      return;
    }
    for (size_t i = 0; i < capacity_; ++i) {
      slots_[i].clear();
    }
    size_ = 0;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::insert(const Key & key, const Value & value)
  {
    size_t index = hasher_(key) % capacity_;
    typename List< Pair >::LIter tail;
    if (!slots_[index].empty()) {
      List< Pair > & basket = slots_[index];
      auto current = basket.begin();
      for (; current != basket.end(); ++current) {
        if (is_equal_(current->first, key)) {
          current->second = value;
          return;
        }
      }
      tail = current;
    } else {
      tail = slots_[index].before_begin();
    }
    slots_[index].insert_after(tail, std::pair< Key, Value >(key, value));
    ++size_;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::remove(const Key & key)
  {
    auto pre_node = find_previous_node(key);
    if (pre_node.first == capacity() + 1) {
      return;
    }
    slots_[pre_node.first].erase_after(pre_node.second);
    --size_;
  }

  template< class Key, class Value, class Hash, class Equal >
  Value & HashTable< Key, Value, Hash, Equal >::at(const Key & key)
  {
    return const_cast< Value & >(static_cast< const HashTable * >(this)->at(key));
  }

  template< class Key, class Value, class Hash, class Equal >
  const Value & HashTable< Key, Value, Hash, Equal >::at(const Key & key) const
  {
    auto found = find_previous_node(key);
    if (found.first == capacity() + 1) {
      throw std::out_of_range("Key not found");
    }
    return (++found.second)->second;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::contains(const Key & key) const
  {
    return (*static_cast< const HashTable * >(this)).find_previous_node(key).first < capacity() + 1;
  }

  template< class Key, class Value, class Hash, class Equal >
  Vector< Key > HashTable< Key, Value, Hash, Equal >::keys() const
  {
    Vector< Key > res;
    for (size_t i = 0; i < capacity_; ++i) {
      if (!slots_[i].empty()) {
        for (auto it = slots_[i].begin(); it != slots_[i].end(); ++it) {
          res.push_back((*it).first);
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
      if (!slots_[i].empty()) {
        for (auto it = slots_[i].begin(); it != slots_[i].end(); ++it) {
          temp.insert((*it).first, (*it).second);
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
  std::pair< size_t, typename List< std::pair< Key, Value > >::LIter >
  HashTable< Key, Value, Hash, Equal >::find_previous_node(const Key & key)
  {
    size_t index = hasher_(key) % capacity_;
    List< Pair > & basket = slots_[index];
    auto pre = basket.before_begin();
    for (auto it = basket.begin(); it != basket.end(); ++it) {
      if (it->first == key) {
        return std::make_pair(index, pre);
      }
      ++pre;
    }
    return std::make_pair(capacity_ + 1, basket.end());
  }

  template< class Key, class Value, class Hash, class Equal >
  std::pair< size_t, typename List< std::pair< Key, Value > >::LCIter >
  HashTable< Key, Value, Hash, Equal >::find_previous_node(const Key & key) const
  {
    size_t index = hasher_(key) % capacity_;
    const List< Pair > & basket = slots_[index];
    auto pre = basket.before_begin();
    for (auto it = basket.begin(); it != basket.end(); ++it) {
      if (it->first == key) {
        return std::make_pair(index, pre);
      }
      ++pre;
    }
    return std::make_pair(capacity_ + 1, basket.end());
  }
}
#endif
