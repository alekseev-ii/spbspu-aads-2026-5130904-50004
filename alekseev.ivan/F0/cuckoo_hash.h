#ifndef CUCKOO_HASH_H
#define CUCKOO_HASH_H

#include <utility>
#include <cmath>
#include "vector.h"

namespace alekseev {
  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  struct CuckooHash
  {
    CuckooHash(Hash1 h1, Hash2 h2, Equal e, size_t cap = 128, double max_load_factor = 0.5);
    ~CuckooHash();
    CuckooHash(const CuckooHash & rhs);
    CuckooHash & operator=(const CuckooHash & rhs);
    CuckooHash(CuckooHash && rhs) noexcept;
    CuckooHash & operator=(CuckooHash && rhs) noexcept;

    void swap(CuckooHash & rhs) noexcept;
    void rehash(size_t new_cap = 0);
    void reserve(size_t cap);

    template< class Forward_Key, class Forward_Value >
    void insert(Forward_Key && k, Forward_Value && v);
    template< class Forward_Key >
    void remove(Forward_Key && k) noexcept;
    template< class Forward_Key >
    Value & at(Forward_Key && k);
    template< class Forward_Key >
    const Value & at(Forward_Key && k) const;
    template< class Forward_Key >
    bool contains(Forward_Key && k) const noexcept;

    Vector< Key > keys() const;
    size_t size() const noexcept;
    size_t capacity() const noexcept;
    double load_factor() const noexcept;
    bool empty() const noexcept;
    void clear() noexcept;
    void set_max_load_factor(double max_load_factor) noexcept;

    struct KeyIterator: std::iterator< std::forward_iterator_tag, Key, std::ptrdiff_t, const Key *,
          const Key & >
    {
      using iterator_category = std::forward_iterator_tag;
      using value_type = Key;
      using difference_type = std::ptrdiff_t;
      using pointer = const Key *;
      using reference = const Key &;

      KeyIterator(bool first_table, size_t index, const Vector< std::pair< Key, Value > * > & t1,
          const Vector< std::pair< Key, Value > * > & t2);

      reference operator*() const noexcept;
      pointer operator->() const noexcept;
      bool operator==(const KeyIterator & rhs) const noexcept;
      bool operator!=(const KeyIterator & rhs) const noexcept;
      bool operator<(const KeyIterator & rhs) const noexcept;
      bool operator<=(const KeyIterator & rhs) const noexcept;
      bool operator>(const KeyIterator & rhs) const noexcept;
      bool operator>=(const KeyIterator & rhs) const noexcept;

      KeyIterator & operator++();
      KeyIterator operator++(int);

      private:
        bool first_table_;
        size_t index_;
        const Vector< std::pair< Key, Value > * > & t1_, & t2_;
    };

    KeyIterator begin() const;
    KeyIterator end() const;
    KeyIterator сbegin() const;
    KeyIterator сend() const;

    private:
      Vector< std::pair< Key, Value > * > table1_;
      Hash1 hasher1_;
      Vector< std::pair< Key, Value > * > table2_;
      Hash2 hasher2_;
      Equal equal_;
      size_t size_;
      size_t capacity_;
      double max_load_factor_;
  };

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::CuckooHash(Hash1 h1, Hash2 h2, Equal e, size_t cap,
      double max_load_factor):
    table1_((cap + 1) / 2, nullptr),
    hasher1_(h1),
    table2_((cap + 1) / 2, nullptr),
    hasher2_(h2),
    equal_(e),
    size_(0),
    capacity_(cap + (cap % 2)),
    max_load_factor_(max_load_factor)
  { }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::~CuckooHash()
  {
    clear();
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::CuckooHash(const CuckooHash & rhs):
    table1_(rhs.capacity() / 2, nullptr),
    hasher1_(rhs.hasher1_),
    table2_(rhs.capacity() / 2, nullptr),
    hasher2_(rhs.hasher2_),
    equal_(rhs.equal_),
    size_(rhs.size_),
    capacity_(rhs.capacity_),
    max_load_factor_(rhs.max_load_factor_)
  {
    for (size_t i = 0; i < (rhs.capacity_ / 2); ++i) {
      try {
        if (rhs.table1_[i] != nullptr) {
          table1_[i] = new std::pair< Key, Value >(*rhs.table1_[i]);
        }
        if (rhs.table2_[i] != nullptr) {
          table2_[i] = new std::pair< Key, Value >(*rhs.table2_[i]);
        }
      } catch (...) {
        clear();
        throw;
      }
    }
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  CuckooHash< Key, Value, Hash1, Hash2, Equal > & CuckooHash< Key, Value, Hash1, Hash2,
    Equal >::operator=(const CuckooHash & rhs)
  {
    CuckooHash temp(rhs);
    swap(temp);
    return *this;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::CuckooHash(CuckooHash && rhs) noexcept:
    table1_(),
    hasher1_(rhs.hasher1_),
    table2_(),
    hasher2_(rhs.hasher2_),
    equal_(rhs.equal_),
    size_(0),
    capacity_(0),
    max_load_factor_(rhs.max_load_factor_)
  {
    swap(rhs);
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  CuckooHash< Key, Value, Hash1, Hash2, Equal > & CuckooHash< Key, Value, Hash1, Hash2,
    Equal >::operator=(CuckooHash && rhs) noexcept
  {
    CuckooHash temp(std::move(rhs));
    swap(temp);
    return *this;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  void CuckooHash< Key, Value, Hash1, Hash2, Equal >::swap(CuckooHash & rhs) noexcept
  {
    table1_.swap(rhs.table1_);
    std::swap(hasher1_, rhs.hasher1_);
    table2_.swap(rhs.table2_);
    std::swap(hasher2_, rhs.hasher2_);
    std::swap(equal_, rhs.equal_);
    std::swap(size_, rhs.size_);
    std::swap(capacity_, rhs.capacity_);
    std::swap(max_load_factor_, rhs.max_load_factor_);
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  void CuckooHash< Key, Value, Hash1, Hash2, Equal >::rehash(size_t new_cap)
  {
    if (new_cap == 0) {
      new_cap = std::pow(2, std::ceil(std::log2(size_ / max_load_factor_)) + 1);
    }
    if (static_cast< double >(size_) / static_cast< double >(new_cap) > max_load_factor_) {
      return;
    }
    CuckooHash temp(hasher1_, hasher2_, equal_, new_cap);
    for (size_t i = 0; i < (capacity_ / 2); ++i) {
      if (table1_[i] != nullptr) {
        std::pair< Key, Value > e = *table1_[i];
        temp.insert(e.first, e.second);
      }
      if (table2_[i] != nullptr) {
        std::pair< Key, Value > e = *table2_[i];
        temp.insert(e.first, e.second);
      }
    }
    swap(temp);
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  void CuckooHash< Key, Value, Hash1, Hash2, Equal >::reserve(size_t cap)
  {
    rehash(cap);
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  template< class Forward_Key, class Forward_Value >
  void CuckooHash< Key, Value, Hash1, Hash2, Equal >::insert(Forward_Key && k, Forward_Value && v)
  {
    size_t pos1 = hasher1_(std::forward< Forward_Key >(k)) % (capacity() / 2);
    if (table1_[pos1] != nullptr) {
      if (equal_(table1_[pos1]->first, std::forward< Forward_Key >(k))) {
        table1_[pos1]->second = std::forward< Forward_Value >(v);
        return;
      }
    }
    size_t pos2 = hasher2_(std::forward< Forward_Key >(k)) % (capacity() / 2);
    if (table2_[pos2] != nullptr) {
      if (equal_(table2_[pos2]->first, std::forward< Forward_Key >(k))) {
        table2_[pos2]->second = std::forward< Forward_Value >(v);
        return;
      }
    }
    CuckooHash temp(*this);
    if ((temp.size() + 1.0) / static_cast< double >(temp.capacity_) > temp.max_load_factor_) {
      temp.rehash(temp.capacity() * 2);
    }
    auto * new_element = new std::pair< Key, Value >(std::forward< Forward_Key >(k),
        std::forward< Forward_Value >(v));
    size_t tries = 0;
    while (tries < 3) {
      pos1 = temp.hasher1_(new_element->first) % (temp.capacity() / 2);
      if (temp.table1_[pos1] == nullptr) {
        temp.table1_[pos1] = new_element;
        ++temp.size_;
        swap(temp);
        return;
      }
      std::pair< Key, Value > * victim = temp.table1_[pos1];
      temp.table1_[pos1] = new_element;
      for (size_t i = 0; i < 16; ++i) {
        pos2 = temp.hasher2_(victim->first) % (temp.capacity() / 2);
        if (temp.table2_[pos2] == nullptr) {
          temp.table2_[pos2] = victim;
          ++temp.size_;
          swap(temp);
          return;
        }
        std::swap(temp.table2_[pos2], victim);

        pos1 = temp.hasher1_(victim->first) % (temp.capacity() / 2);
        if (temp.table1_[pos1] == nullptr) {
          temp.table1_[pos1] = victim;
          ++temp.size_;
          swap(temp);
          return;
        }
        std::swap(temp.table1_[pos1], victim);
      }
      new_element = victim;
      temp.rehash(temp.capacity() * 2);
      ++tries;
    }
    throw std::logic_error("Cannot add element");
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  template< class Forward_Key >
  void CuckooHash< Key, Value, Hash1, Hash2, Equal >::remove(Forward_Key && k) noexcept
  {
    size_t pos1 = hasher1_(std::forward< Forward_Key >(k)) % (capacity() / 2);
    if (table1_[pos1] != nullptr) {
      if (equal_(table1_[pos1]->first, std::forward< Forward_Key >(k))) {
        delete table1_[pos1];
        table1_[pos1] = nullptr;
        --size_;
        return;
      }
    }
    size_t pos2 = hasher2_(std::forward< Forward_Key >(k)) % (capacity() / 2);
    if (table2_[pos2] != nullptr) {
      if (equal_(table2_[pos2]->first, std::forward< Forward_Key >(k))) {
        delete table2_[pos2];
        table2_[pos2] = nullptr;
        --size_;
        return;
      }
    }
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  template< class Forward_Key >
  Value & CuckooHash< Key, Value, Hash1, Hash2, Equal >::at(Forward_Key && k)
  {
    return const_cast< Value & >(static_cast< const CuckooHash * >(this)->at(
        std::forward< Forward_Key >(k)));
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  template< class Forward_Key >
  const Value & CuckooHash< Key, Value, Hash1, Hash2, Equal >::at(Forward_Key && k) const
  {
    size_t pos1 = hasher1_(std::forward< Forward_Key >(k)) % (capacity() / 2);
    if (table1_[pos1] != nullptr) {
      if (equal_(table1_[pos1]->first, std::forward< Forward_Key >(k))) {
        return table1_[pos1]->second;
      }
    }
    size_t pos2 = hasher2_(std::forward< Forward_Key >(k)) % (capacity() / 2);
    if (table2_[pos2] != nullptr) {
      if (equal_(table2_[pos2]->first, std::forward< Forward_Key >(k))) {
        return table2_[pos2]->second;
      }
    }
    throw std::out_of_range("Key not found");
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  template< class Forward_Key >
  bool CuckooHash< Key, Value, Hash1, Hash2, Equal >::contains(Forward_Key && k) const noexcept
  {
    size_t pos1 = hasher1_(std::forward< Forward_Key >(k)) % (capacity() / 2);
    if (table1_[pos1] != nullptr) {
      if (equal_(table1_[pos1]->first, std::forward< Forward_Key >(k))) {
        return true;
      }
    }
    size_t pos2 = hasher2_(std::forward< Forward_Key >(k)) % (capacity() / 2);
    if (table2_[pos2] != nullptr) {
      if (equal_(table2_[pos2]->first, std::forward< Forward_Key >(k))) {
        return true;
      }
    }
    return false;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  Vector< Key > CuckooHash< Key, Value, Hash1, Hash2, Equal >::keys() const
  {
    Vector< Key > result;
    result.resize(size());
    for (size_t i = 0; i < capacity() / 2; ++i) {
      if (table1_[i] != nullptr) {
        result.pushBack(table1_[i]->first);
      }
      if (table2_[i] != nullptr) {
        result.pushBack(table2_[i]->first);
      }
    }
    return result;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  size_t CuckooHash< Key, Value, Hash1, Hash2, Equal >::size() const noexcept
  {
    return size_;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  size_t CuckooHash< Key, Value, Hash1, Hash2, Equal >::capacity() const noexcept
  {
    return capacity_;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  double CuckooHash< Key, Value, Hash1, Hash2, Equal >::load_factor() const noexcept
  {
    return static_cast< double >(size_) / static_cast< double >(capacity_);
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  bool CuckooHash< Key, Value, Hash1, Hash2, Equal >::empty() const noexcept
  {
    return size_ == 0;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  void CuckooHash< Key, Value, Hash1, Hash2, Equal >::clear() noexcept
  {
    for (size_t i = 0; i < (capacity_ / 2); ++i) {
      if (table1_[i] != nullptr) {
        delete table1_[i];
        table1_[i] = nullptr;
      }
      if (table2_[i] != nullptr) {
        delete table2_[i];
        table2_[i] = nullptr;
      }
    }
    size_ = 0;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  void CuckooHash< Key, Value, Hash1, Hash2, Equal >::set_max_load_factor(
      double max_load_factor) noexcept
  {
    max_load_factor_ = max_load_factor;
    if (load_factor() > max_load_factor_) {
      rehash();
    }
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator::KeyIterator(bool first_table,
      size_t index, const Vector< std::pair< Key, Value > * > & t1,
      const Vector< std::pair< Key, Value > * > & t2):
    first_table_(first_table),
    index_(index),
    t1_(t1),
    t2_(t2)
  { }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  typename CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator::reference
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator::operator*() const noexcept
  {
    if (first_table_) {
      return t1_[index_]->first;
    }
    return t2_[index_]->first;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  typename CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator::pointer
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator::operator->() const noexcept
  {
    if (first_table_) {
      return std::addressof(t1_[index_]->first);
    }
    return std::addressof(t2_[index_]->first);
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  bool CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator::operator==(
      const KeyIterator & rhs) const noexcept
  {
    return first_table_ == rhs.first_table_ && index_ == rhs.index_;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  bool CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator::operator!=(
      const KeyIterator & rhs) const noexcept
  {
    return !(*this == rhs);
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  bool CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator::operator<(
      const KeyIterator & rhs) const noexcept
  {
    if (first_table_ == rhs.first_table_) {
      return index_ < rhs.index_;
    }
    return first_table_ > rhs.first_table_;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  bool CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator::operator<=(
      const KeyIterator & rhs) const noexcept
  {
    return *this < rhs || *this == rhs;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  bool CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator::operator>(
      const KeyIterator & rhs) const noexcept
  {
    return !(*this <= rhs);
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  bool CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator::operator>=(
      const KeyIterator & rhs) const noexcept
  {
    return !(*this < rhs);
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  typename CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator &
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator::operator++()
  {
    ++index_;
    while (first_table_ && index_ < t1_.getSize()) {
      if (t1_[index_] == nullptr) {
        ++index_;
      } else {
        return *this;
      }
    }
    if (first_table_) {
      first_table_ = false;
      index_ = 0;
    }
    while (index_ < t2_.getSize()) {
      if (t2_[index_] == nullptr) {
        ++index_;
      } else {
        return *this;
      }
    }
    return *this;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  typename CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator::operator++(int)
  {
    KeyIterator tmp = *this;
    ++(*this);
    return tmp;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  typename CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::begin() const
  {
    for (size_t i = 0; i < (capacity_ / 2); ++i) {
      if (table1_[i] != nullptr) {
        return KeyIterator(true, i, table1_, table2_);
      }
    }
    for (size_t i = 0; i < (capacity_ / 2); ++i) {
      if (table2_[i] != nullptr) {
        return KeyIterator(false, i, table1_, table2_);
      }
    }
    return end();
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  typename CuckooHash< Key, Value, Hash1, Hash2, Equal >::KeyIterator CuckooHash< Key, Value, Hash1,
    Hash2, Equal >::end() const
  {
    return KeyIterator(false, capacity_ / 2, table1_, table2_);
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  typename CuckooHash<Key, Value, Hash1, Hash2, Equal>::KeyIterator CuckooHash<Key, Value, Hash1,
  Hash2, Equal>::сbegin() const
  {
    return begin();
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  typename CuckooHash<Key, Value, Hash1, Hash2, Equal>::KeyIterator CuckooHash<Key, Value, Hash1,
  Hash2, Equal>::сend() const
  {
    return end();
  }
}

#endif
