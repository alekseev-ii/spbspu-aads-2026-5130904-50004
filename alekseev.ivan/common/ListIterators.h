#ifndef LISTITERATORS_H
#define LISTITERATORS_H

#include "List.h"
#include <memory>

namespace alekseev {
  template< class T, class Derived >
  struct ListIteratorBase;
  template< class T >
  struct LIter;
  template< class T >
  struct LCIter;

  template< class T, class U >
  LIter< T > insert_after(LIter< T > & element, U && value);
  template< class T >
  LIter< T > erase_after(LIter< T > & element);
  template< class T >
  LIter< T > clear(LIter< T > & start, LIter< T > & end);
  template< class T >
  LIter< T > begin(List< T > * fake_node);
  template< class T >
  LIter< T > before_begin(List< T > * fake_node);
  template< class T >
  LIter< T > end(List< T > * fake_node);
  template< class T >
  LCIter< T > begin(const List< T > * fake_node);
  template< class T >
  LCIter< T > before_begin(const List< T > * fake_node);
  template< class T >
  LCIter< T > end(const List< T > * fake_node);
  template< class Iter >
  Iter next(Iter it);
  template< class Iter >
  void destroy(Iter iterator_on_fake);

  template< class T, class Derived >
  struct ListIteratorBase
  {
    List< T > * node_;

    friend class List< T >;

    ListIteratorBase();

    explicit ListIteratorBase(List< T > * node);

    Derived & operator++();
    Derived operator++(int);

    template< class OtherDerived >
    bool operator==(const ListIteratorBase< T, OtherDerived > & other) const;

    template< class OtherDerived >
    bool operator!=(const ListIteratorBase< T, OtherDerived > & other) const;
  };

  template< class T, class Derived >
  ListIteratorBase< T, Derived >::ListIteratorBase():
    node_(nullptr)
  { }

  template< class T, class Derived >
  ListIteratorBase< T, Derived >::ListIteratorBase(List< T > * node):
    node_(node)
  { }

  template< class T, class Derived >
  Derived & ListIteratorBase< T, Derived >::operator++()
  {
    node_ = node_->next;
    return static_cast< Derived & >(*this);
  }

  template< class T, class Derived >
  Derived ListIteratorBase< T, Derived >::operator++(int)
  {
    Derived tmp = static_cast< Derived & >(*this);
    ++(*this);
    return tmp;
  }

  template< class T, class Derived >
  template< class OtherDerived >
  bool ListIteratorBase< T, Derived >::operator==(
      const ListIteratorBase< T, OtherDerived > & other) const
  {
    return node_ == other.node_;
  }

  template< class T, class Derived >
  template< class OtherDerived >
  bool ListIteratorBase< T, Derived >::operator!=(
      const ListIteratorBase< T, OtherDerived > & other) const
  {
    return !(*this == other);
  }

  template< class T >
  struct LIter: ListIteratorBase< T, LIter< T > >,
      std::iterator< std::forward_iterator_tag, T, std::ptrdiff_t, T *, T & >
  {
    using ListIteratorBase< T, LIter< T > >::ListIteratorBase;

    LIter(const ListIteratorBase< T, LIter< T > > & base);

    T & operator*() const;
    T * operator->() const;
  };

  template< class T >
  LIter< T >::LIter(const ListIteratorBase< T, LIter< T > > & base):
    ListIteratorBase< T, LIter< T > >(base)
  { }

  template< class T >
  T & LIter< T >::operator*() const
  {
    return this->node_->data;
  }

  template< class T >
  T * LIter< T >::operator->() const
  {
    return std::addressof(this->node_->data);
  }

  template< class T, class U >
  LIter< T > insert_after(LIter< T > & element, U && value)
  {
    element.node_ = insert_after(element.node_, std::forward< U >(value));
    return element;
  }

  template< class T >
  LIter< T > erase_after(LIter< T > & element)
  {
    element.node_ = erase_after(element.node_);
    return element;
  }

  template< class T >
  LIter< T > clear(LIter< T > & start, LIter< T > & end)
  {
    start.node_ = clear(start.node_, end.node_);
    return start;
  }

  template< class T >
  struct LCIter: ListIteratorBase< T, LCIter< T > >,
      std::iterator< std::forward_iterator_tag, T, std::ptrdiff_t, const T *, const T & >
  {
    using ListIteratorBase< T, LCIter< T > >::ListIteratorBase;

    LCIter(const LIter< T > & rhs);
    LCIter(const ListIteratorBase< T, LCIter< T > > & base);

    const T & operator*() const;
    const T * operator->() const;
  };

  template< class T >
  LCIter< T >::LCIter(const LIter< T > & rhs):
    ListIteratorBase< T, LCIter< T > >(rhs.node_)
  { }

  template< class T >
  LCIter< T >::LCIter(const ListIteratorBase< T, LCIter< T > > & base):
    ListIteratorBase< T, LCIter< T > >(base)
  { }

  template< class T >
  const T & LCIter< T >::operator*() const
  {
    return this->node_->data;
  }

  template< class T >
  const T * LCIter< T >::operator->() const
  {
    return std::addressof(this->node_->data);
  }

  template< class T >
  LIter< T > begin(List< T > * fake_node)
  {
    return LIter< T >(fake_node->next);
  }

  template< class T >
  LIter< T > before_begin(List< T > * fake_node)
  {
    return LIter< T >(fake_node);
  }

  template< class T >
  LIter< T > end(List< T > * fake_node)
  {
    return LIter< T >(fake_node);
  }

  template< class T >
  LCIter< T > begin(const List< T > * fake_node)
  {
    return LCIter< T >(fake_node->next);
  }

  template< class T >
  LCIter< T > before_begin(const List< T > * fake_node)
  {
    return LCIter< T >(fake_node);
  }

  template< class T >
  LCIter< T > end(const List< T > * fake_node)
  {
    return LCIter< T >(fake_node);
  }

  template< class Iter >
  Iter next(Iter it)
  {
    return ++it;
  }

  template< class Iter >
  void destroy(Iter iterator_on_fake)
  {
    Iter end = iterator_on_fake;
    clear(++iterator_on_fake, end);
    rmfake(end.node_);
  }
}

#endif
