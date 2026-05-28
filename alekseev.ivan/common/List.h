#ifndef LIST_H
#define LIST_H

#include <utility>

namespace alekseev {
  template< class T >
  struct List
  {
    T data;
    List< T > * next;
  };

  template< class T >
  List< T > * fake(List< T > * head = nullptr);
  template< class T >
  List< T > * rmfake(List< T > * fake_node);
  template< class T, class U >
  List< T > * insert_after(List< T > * element, U && value);
  template< class T >
  List< T > * erase_after(List< T > * element);
  template< class T >
  List< T > * clear(List< T > * start, List< T > * end);

  template< class T >
  List< T > * fake(List< T > * head)
  {
    List< T > * fake_node = static_cast< List< T > * >(::operator new(sizeof(List< T >)));
    if (head) {
      fake_node->next = head;
      head->next = fake_node;
    } else {
      fake_node->next = fake_node;
    }
    return fake_node;
  }

  template< class T >
  List< T > * rmfake(List< T > * fake_node)
  {
    List< T > * head = fake_node->next;
    ::operator delete(fake_node);
    return head;
  }

  template< class T, class U >
  List< T > * insert_after(List< T > * element, U && value)
  {
    List< T > * new_node = new List< T >();
    try {
      new_node->data = std::forward< U >(value);
    } catch (...) {
      delete new_node;
      throw;
    }
    new_node->next = element->next;
    element->next = new_node;
    return new_node;
  }

  template< class T >
  List< T > * erase_after(List< T > * element)
  {
    List< T > * tmp = element->next->next;
    delete element->next;
    element->next = tmp;
    return element->next;
  }

  template< class T >
  List< T > * clear(List< T > * start, List< T > * end)
  {
    while (start != end) {
      List< T > * tmp = start->next;
      delete start;
      start = tmp;
    }
    return start;
  }
}

#endif
