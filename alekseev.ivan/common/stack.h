#ifndef STACK_H
#define STACK_H

#include "List.h"

namespace alekseev {
  template< class T >
  struct Stack
  {
    Stack();
    ~Stack() = default;
    Stack(const Stack & rhs) = default;
    Stack & operator=(const Stack & rhs) = default;
    Stack(Stack && rhs) noexcept = default;
    Stack & operator=(Stack && rhs) noexcept = default;

    template< class U >
    void push(U && value);

    void pop();
    T & top();
    bool empty() const;
    size_t size() const;
    void clear() noexcept;

    private:
      List< T > stack_;
  };

  template< class T >
  Stack< T >::Stack():
    stack_()
  { }

  template< class T >
  template< class U >
  void Stack< T >::push(U && value)
  {
    stack_.push_front(std::forward< U >(value));
  }

  template< class T >
  void Stack< T >::pop()
  {
    stack_.pop_front();
  }

  template< class T >
  T & Stack< T >::top()
  {
    return stack_.front();
  }

  template< class T >
  bool Stack< T >::empty() const
  {
    return stack_.empty();
  }

  template< class T >
  size_t Stack< T >::size() const
  {
    return stack_.size();
  }

  template< class T >
  void Stack< T >::clear() noexcept
  {
    stack_.clear();
  }
}

#endif
