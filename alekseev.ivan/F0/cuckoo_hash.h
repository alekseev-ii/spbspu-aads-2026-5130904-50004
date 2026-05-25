#ifndef CUCKOO_HASH_H
#define CUCKOO_HASH_H

#include <cstddef>
#include <utility>

namespace alekseev {
  template< class Key, class Value, class Hash1, class Hash2, class Equal >
  struct CuckooHash {
    CuckooHash(Hash1 h1, Hash2 h2, Equal e);
    ~CuckooHash();
    CuckooHash(CuckooHash const & rhs);
    CuckooHash & operator=(CuckooHash const & rhs);
    CuckooHash(CuckooHash && rhs);
    CuckooHash & operator=(CuckooHash && rhs);

    swap(CuckooHash & rhs);
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
      std::pair< Key, Value > ** table1;
      Hash1 hasher1_;
      std::pair< Key, Value > ** table2;
      Hash2 hasher2_;
      Equal equal_;
      size_t size_;
      size_t capacity_;
  };
}

#endif
