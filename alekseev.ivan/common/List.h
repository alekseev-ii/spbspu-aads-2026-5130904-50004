#ifndef LIST_H
#define LIST_H

#include <iterator>

namespace alekseev {
  template< class T >
  struct Node
  {
    T data_;
    Node * next_;
  };

  template< class T >
  struct List
  {
    List();
    ~List();
    List(const List & rhs);
    List & operator=(const List & rhs);
    List(List && rhs) noexcept;
    List & operator=(List && rhs) noexcept;

    void clear() noexcept;
    void swap(List & rhs) noexcept;

    T & front();
    const T & front() const;
    void pop_front() noexcept;
    template< class U >
    void push_front(U && data) noexcept;

    bool empty() const;
    size_t size() const;

    struct LIter: std::iterator< std::forward_iterator_tag, T >
    {
      friend class List;
      LIter(Node< T > * current, Node< T > * fake_node);

      LIter & operator++();
      LIter operator++(int);
      bool operator==(const LIter & rhs) const;
      bool operator!=(const LIter & rhs) const;
      T & operator*();
      T * operator->();

      private:
        Node< T > * current_;
        Node< T > * fake_node_;
    };

    struct LCIter: std::iterator< std::forward_iterator_tag, T, std::ptrdiff_t, const T *, const T
          & >
    {
      friend class List;
      LCIter(Node< T > * current, Node< T > * fake_node);
      LCIter(LIter rhs);

      LCIter & operator++();
      LCIter operator++(int);
      bool operator==(const LCIter & rhs) const;
      bool operator!=(const LCIter & rhs) const;
      const T & operator*() const;
      const T * operator->() const;

      private:
        Node< T > * current_;
        Node< T > * fake_node_;
    };

    LIter before_begin();
    LIter begin();
    LIter end();
    LCIter before_begin() const;
    LCIter begin() const;
    LCIter end() const;

    template< class U >
    void insert_after(LIter pos, U && data);
    void erase_after(LIter pos);

    private:
      Node< T > * fake_node_;
      size_t size_;
  };

  template< class T >
  List< T >::List():
    fake_node_(static_cast< Node< T > * >(::operator new(sizeof(Node< T >)))),
    size_(0)
  {
    fake_node_->next_ = fake_node_;
  }

  template< class T >
  List< T >::~List()
  {
    clear();
    if (fake_node_ != nullptr) {
      ::operator delete(fake_node_);
    }
  }

  template< class T >
  List< T >::List(const List & rhs):
    List()
  {
    auto current = before_begin();
    for (LCIter it = rhs.begin(); it != rhs.end(); ++it) {
      insert_after(current, *it);
      ++current;
    }
  }

  template< class T >
  List< T > & List< T >::operator=(const List & rhs)
  {
    if (this == std::addressof(rhs)) {
      return *this;
    }
    List temp(rhs);
    swap(temp);
    return *this;
  }

  template< class T >
  List< T >::List(List && rhs) noexcept:
    List()
  {
    swap(rhs);
  }

  template< class T >
  List< T > & List< T >::operator=(List && rhs) noexcept
  {
    if (this == std::addressof(rhs)) {
      return *this;
    }
    List temp(std::move(rhs));
    swap(temp);
    return *this;
  }

  template< class T >
  void List< T >::clear() noexcept
  {
    while (!empty()) {
      pop_front();
    }
  }

  template< class T >
  void List< T >::swap(List & rhs) noexcept
  {
    std::swap(fake_node_, rhs.fake_node_);
    std::swap(size_, rhs.size_);
  }

  template< class T >
  T & List< T >::front()
  {
    return fake_node_->next_->data_;
  }

  template< class T >
  const T & List< T >::front() const
  {
    return fake_node_->next_->data_;
  }

  template< class T >
  void List< T >::pop_front() noexcept
  {
    if (empty()) {
      return;
    }
    Node< T > * next = fake_node_->next_->next_;
    delete fake_node_->next_;
    fake_node_->next_ = next;
    --size_;
  }

  template< class T >
  template< class U >
  void List< T >::push_front(U && data) noexcept
  {
    insert_after(before_begin(), std::forward< U >(data));
  }

  template< class T >
  bool List< T >::empty() const
  {
    return size_ == 0;
  }

  template< class T >
  size_t List< T >::size() const
  {
    return size_;
  }

  template< class T >
  List< T >::LIter::LIter(Node< T > * current, Node< T > * fake_node):
    current_(current),
    fake_node_(fake_node)
  { }

  template< class T >
  typename List< T >::LIter & List< T >::LIter::operator++()
  {
    current_ = current_->next_;
    return *this;
  }

  template< class T >
  typename List< T >::LIter List< T >::LIter::operator++(int)
  {
    LIter tmp = *this;
    ++(*this);
    return tmp;
  }

  template< class T >
  bool List< T >::LIter::operator==(const LIter & rhs) const
  {
    return current_ == rhs.current_;
  }

  template< class T >
  bool List< T >::LIter::operator!=(const LIter & rhs) const
  {
    return !(*this == rhs);
  }

  template< class T >
  T & List< T >::LIter::operator*()
  {
    return current_->data_;
  }

  template< class T >
  T * List< T >::LIter::operator->()
  {
    return std::addressof(current_->data_);
  }

  template< class T >
  List< T >::LCIter::LCIter(Node< T > * current, Node< T > * fake_node):
    current_(current),
    fake_node_(fake_node)
  { }

  template< class T >
  List< T >::LCIter::LCIter(LIter rhs):
    LCIter(rhs.current_, rhs.fake_node_)
  { }

  template< class T >
  typename List< T >::LCIter & List< T >::LCIter::operator++()
  {
    current_ = current_->next_;
    return *this;
  }

  template< class T >
  typename List< T >::LCIter List< T >::LCIter::operator++(int)
  {
    LCIter tmp = *this;
    ++(*this);
    return tmp;
  }

  template< class T >
  bool List< T >::LCIter::operator==(const LCIter & rhs) const
  {
    return current_ == rhs.current_;
  }

  template< class T >
  bool List< T >::LCIter::operator!=(const LCIter & rhs) const
  {
    return !(*this == rhs);
  }

  template< class T >
  const T & List< T >::LCIter::operator*() const
  {
    return current_->data_;
  }

  template< class T >
  const T * List< T >::LCIter::operator->() const
  {
    return std::addressof(current_->data_);
  }

  template< class T >
  typename List< T >::LIter List< T >::before_begin()
  {
    return LIter(fake_node_, fake_node_);
  }

  template< class T >
  typename List< T >::LIter List< T >::begin()
  {
    return ++before_begin();
  }

  template< class T >
  typename List< T >::LIter List< T >::end()
  {
    return before_begin();
  }

  template< class T >
  typename List< T >::LCIter List< T >::before_begin() const
  {
    return LCIter(fake_node_, fake_node_);
  }

  template< class T >
  typename List< T >::LCIter List< T >::begin() const
  {
    return ++before_begin();
  }

  template< class T >
  typename List< T >::LCIter List< T >::end() const
  {
    return before_begin();
  }

  template< class T >
  template< class U >
  void List< T >::insert_after(LIter pos, U && data)
  {
    Node< T > * current = pos.current_;
    current->next_ = new Node< T >{std::forward< U >(data), current->next_};
    ++size_;
  }

  template< class T >
  void List< T >::erase_after(LIter pos)
  {
    Node< T > * current = pos.current_;
    Node< T > * next = current->next_->next_;
    delete current->next_;
    current->next_ = next;
    --size_;
  }
}

#endif
