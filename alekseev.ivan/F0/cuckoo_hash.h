#ifndef CUCKOO_HASH_H
#define CUCKOO_HASH_H

#include <utility>
#include "../common/vector.h"

namespace alekseev {
  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  struct CuckooHash {
    CuckooHash(Hash1 h1, Hash2 h2, Equal e);
    ~CuckooHash();
    CuckooHash(const CuckooHash & rhs);
    CuckooHash & operator=(const CuckooHash & rhs);
    CuckooHash(CuckooHash && rhs) noexcept;
    CuckooHash & operator=(CuckooHash && rhs) noexcept;

    void swap(CuckooHash & rhs) noexcept;
    void rehash();
    void push(Key & k, Value & v);
    void remove(Key & k);
    Value & at(const Key & k);
    const Value & at(const Key & k) const;
    size_t size() const;
    size_t capacity() const;
    double load_factor() const;
    bool empty() const;
    void clear();

    private:
      Vector< std::pair< Key, Value > * > table1_;
      Hash1 hasher1_;
      Vector< std::pair< Key, Value > * > table2_;
      Hash2 hasher2_;
      Equal equal_;
      size_t size_;
      size_t capacity_;
  };

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::CuckooHash(Hash1 h1, Hash2 h2, Equal e):
    table1_(),
    hasher1_(h1),
    table2_(),
    hasher2_(h2),
    equal_(e),
    size_(0),
    capacity_(0)
  {
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::~CuckooHash()
  {
    clear();
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::CuckooHash(const CuckooHash & rhs):
    table1_(rhs.capacity(), nullptr),
    hasher1_(rhs.hasher1_),
    table2_(rhs.capacity(), nullptr),
    hasher2_(rhs.hasher2_),
    equal_(rhs.equal_),
    size_(rhs.size_),
    capacity_(rhs.capacity_)
  {
    for (size_t i = 0; i < rhs.capacity_; ++i) {
      if (rhs.table1_[i] != nullptr) {
        *table1_[i] = *rhs.table1_[i];
      }
      if (rhs.table2_[i] != nullptr) {
        *table2_[i] = *rhs.table2_[i];
      }
    }
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  CuckooHash< Key, Value, Hash1, Hash2, Equal > & CuckooHash< Key, Value, Hash1, Hash2, Equal >::
  operator=(const CuckooHash & rhs)
  {
    CuckooHash temp(rhs);
    swap(temp);
    return *this;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::CuckooHash(CuckooHash && rhs) noexcept:
    table1_(std::move(rhs.table1_)),
    hasher1_(rhs.hasher1_),
    table2_(std::move(rhs.table2_)),
    hasher2_(rhs.hasher2_),
    equal_(rhs.equal_),
    size_(rhs.size_),
    capacity_(rhs.capacity_)
  {
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  CuckooHash< Key, Value, Hash1, Hash2, Equal > & CuckooHash< Key, Value, Hash1, Hash2, Equal >::
  operator=(CuckooHash && rhs) noexcept
  {
    swap(rhs);
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
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  void CuckooHash< Key, Value, Hash1, Hash2, Equal >::rehash()
  {
    CuckooHash temp(hasher1_, hasher2_, equal_);
    temp.table1_.resize(capacity_ * 2);
    temp.table2_.resize(capacity_ * 2);
    for (size_t i = 0; i < capacity_; ++i) {
      if (table1_[i] != nullptr) {
        std::pair< Key, Value > e = *table1_[i];
        temp.push(e->first, e->second);
      }
      if (table2_[i] != nullptr) {
        std::pair< Key, Value > e = *table2_[i];
        temp.push(e->first, e->second);
      }
    }
    swap(temp);
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  void CuckooHash< Key, Value, Hash1, Hash2, Equal >::push(Key & k, Value & v)
  {
    size_t pos1 = hasher1_(k) % capacity();
    if (table1_[pos1] == nullptr) {
      table1_[pos1] = new std::pair< Key, Value >(k, v);
      ++size_;
      return;
    }
    std::pair< Key, Value > * old = table1_[pos1];
    table1_[pos1] = new std::pair< Key, Value >(k, v);
    for (size_t attempt = 0; attempt < 16; ++attempt) {
      size_t pos2 = hasher2_(old->first) % capacity();
      if (table2_[pos2] == nullptr) {
        table2_[pos2] = old;
        ++size_;
        return;
      }
      std::swap(table2_[pos2], old);

      pos1 = hasher1_(old->first) % capacity();
      if (table1_[pos1] == nullptr) {
        table1_[pos1] = old;
        ++size_;
        return;
      }
      std::swap(table1_[pos1], old);
    }
    try {
      rehash();
    } catch (...) {
      k = old->first;
      v = old->second;
      delete old;
      throw;
    }
    push(old->first, old->second);
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  void CuckooHash< Key, Value, Hash1, Hash2, Equal >::remove(Key & k)
  {
    size_t pos1 = hasher1_(k) % capacity();
    if (table1_[pos1] != nullptr) {
      if (equal_(table1_[pos1]->first, k)) {
        delete table1_[pos1];
        table1_[pos1] = nullptr;
        --size_;
        return;
      }
    }
    size_t pos2 = hasher2_(k) % capacity();
    if (table2_[pos2] != nullptr) {
      if (equal_(table2_[pos2]->first, k)) {
        delete table2_[pos2];
        table2_[pos2] = nullptr;
        --size_;
        return;
      }
    }
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  Value & CuckooHash< Key, Value, Hash1, Hash2, Equal >::at(const Key & k)
  {
    return const_cast< Value & >(static_cast< const CuckooHash * >(this)->at(k));
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  const Value & CuckooHash< Key, Value, Hash1, Hash2, Equal >::at(const Key & k) const
  {
    size_t pos1 = hasher1_(k) % capacity();
    if (table1_[pos1] != nullptr) {
      if (equal_(table1_[pos1]->first, k)) {
        return table1_[pos1]->second;
      }
    }
    size_t pos2 = hasher2_(k) % capacity();
    if (table2_[pos2] != nullptr) {
      if (equal_(table2_[pos2]->first, k)) {
        return table2_[pos2]->second;
      }
    }
    throw std::out_of_range("Key not found");
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  size_t CuckooHash< Key, Value, Hash1, Hash2, Equal >::size() const
  {
    return size_;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  size_t CuckooHash< Key, Value, Hash1, Hash2, Equal >::capacity() const
  {
    return capacity_;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  double CuckooHash< Key, Value, Hash1, Hash2, Equal >::load_factor() const
  {
    return static_cast< double >(size_) / static_cast< double >(capacity_);
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  bool CuckooHash< Key, Value, Hash1, Hash2, Equal >::empty() const
  {
    return size_ == 0;
  }

  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  void CuckooHash< Key, Value, Hash1, Hash2, Equal >::clear()
  {
    for (size_t i = 0; i < capacity_; ++i) {
      if (table1_[i] != nullptr) {
        delete table1_[i];
        table1_[i] = nullptr;
      }
      if (table2_[i] != nullptr) {
        delete table2_[i];
        table2_[i] = nullptr;
      }
    }
    table1_.~Vector();
    table2_.~Vector();
    size_ = 0;
  }
}

#endif
