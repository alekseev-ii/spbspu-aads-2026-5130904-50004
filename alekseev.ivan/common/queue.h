#ifndef QUEUE_H
#define QUEUE_H

#include "List.h"
#include <cstddef>

namespace alekseev {
  template< class T >
  struct Queue
  {
    Queue();
    ~Queue() = default;
    Queue(const Queue & rhs);
    Queue & operator=(const Queue & rhs);
    Queue(Queue && rhs) noexcept;
    Queue & operator=(Queue && rhs) noexcept;

    template< class U >
    void push(U && value);

    void pop();
    T & front();
    const T & front() const;
    T & back();
    const T & back() const;
    bool empty() const;
    size_t size() const;
    void clear() noexcept;

    void swap(Queue & rhs) noexcept;

    private:
      List< T > queue_;
      typename List< T >::LIter tail_;
  };

  template< class T >
  Queue< T >::Queue():
    queue_()
  {
    tail_ = queue_.before_begin();
  }

  template< class T >
  Queue< T >::Queue(const Queue & rhs):
    queue_(rhs.queue_)
  {
    auto it = queue_.begin();
    tail_ = queue_.before_begin();
    for (; it != queue_.end(); ++it) {
      ++tail_;
    }
  }

  template< class T >
  Queue< T > & Queue< T >::operator=(const Queue & rhs)
  {
    if (this == std::addressof(rhs)) {
      return *this;
    }
    Queue temp(rhs);
    swap(temp);
    return *this;
  }

  template< class T >
  Queue< T >::Queue(Queue && rhs) noexcept:
    Queue()
  {
    swap(rhs);
  }

  template< class T >
  Queue< T > & Queue< T >::operator=(Queue && rhs) noexcept
  {
    if (this == std::addressof(rhs)) {
      return *this;
    }
    Queue temp(rhs);
    swap(temp);
    return *this;
  }

  template< class T >
  template< class U >
  void Queue< T >::push(U && value)
  {
    queue_.insert_after(tail_, value);
    ++tail_;
  }

  template< class T >
  void Queue< T >::pop()
  {
    queue_.pop_front();
    if (queue_.empty()) {
      tail_ = queue_.before_begin();
    }
  }

  template< class T >
  T & Queue< T >::front()
  {
    return queue_.front();
  }

  template< class T >
  const T & Queue< T >::front() const
  {
    return queue_.front();
  }

  template< class T >
  T & Queue< T >::back()
  {
    return *tail_;
  }

  template< class T >
  const T & Queue< T >::back() const
  {
    return *tail_;
  }

  template< class T >
  bool Queue< T >::empty() const
  {
    return queue_.empty();
  }

  template< class T >
  size_t Queue< T >::size() const
  {
    return queue_.size();
  }

  template< class T >
  void Queue< T >::clear() noexcept
  {
    queue_.clear();
    tail_ = queue_.before_begin();
  }

  template< class T >
  void Queue< T >::swap(Queue & rhs) noexcept
  {
    queue_.swap(rhs.queue_);
    std::swap(tail_, rhs.tail_);
  }
}

#endif
