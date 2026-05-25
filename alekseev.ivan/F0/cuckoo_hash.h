#ifndef CUCKOO_HASH_H
#define CUCKOO_HASH_H

#include <utility>
#include "../common/vector.h"

namespace alekseev {
  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  struct CuckooHash {
    CuckooHash(Hash1 h1, Hash2 h2, Equal e);
    ~CuckooHash();
    CuckooHash(CuckooHash const & rhs);
    CuckooHash & operator=(CuckooHash const & rhs);
    CuckooHash(CuckooHash && rhs) noexcept;
    CuckooHash & operator=(CuckooHash && rhs) noexcept;

    void swap(CuckooHash & rhs) noexcept;
    void rehash();
    void push(Key const & k, Value const & v);
    Value & at(Key const & k);
    const Value & at(Key const & k) const;
    Value & operator[](Key const & k);
    const Value & operator[](Key const & k) const;
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
  CuckooHash< Key, Value, Hash1, Hash2, Equal >::CuckooHash(CuckooHash const & rhs):
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
  operator=(CuckooHash const & rhs)
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
}

#endif
