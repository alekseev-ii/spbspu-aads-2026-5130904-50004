#ifndef VECTOR_H
#define VECTOR_H

#include <stdexcept>
#include <initializer_list>
#include <iostream>

namespace alekseev {
  template< class T >
  struct Vector
  {
    ~Vector();
    Vector(const Vector & rhs);
    Vector & operator=(const Vector & rhs);
    Vector(Vector && rhs) noexcept;
    Vector & operator=(Vector && rhs) noexcept;

    Vector();
    explicit Vector(std::initializer_list< T > init);
    Vector(size_t size, const T & value);

    bool empty() const noexcept;
    size_t size() const noexcept;
    size_t capacity() const noexcept;

    T & operator[](size_t id) noexcept;
    const T & operator[](size_t id) const noexcept;
    T & at(size_t id);
    const T & at(size_t id) const;
    void swap(Vector & rhs) noexcept;
    void pushFront(const T & value);
    void pushBack(const T &);
    void push_back(const T & value);
    void popBack();
    bool operator==(const Vector & rhs) const;

    void insert(size_t id, const T & value);
    void insert(size_t id, const Vector & rhs, size_t begin, size_t end);
    void erase(size_t id);
    void erase(size_t begin, size_t end);
    Vector operator+(const Vector & rhs) const;
    Vector & operator+=(const Vector & rhs);

    template< class Less >
    void bubbleSort(Less less);
    void resize(size_t new_capacity);

    private:
      explicit Vector(size_t size);
      T * data_;
      size_t size_, capacity_;
  };
}

template< class T >
alekseev::Vector< T >::Vector():
  data_(nullptr),
  size_(0),
  capacity_(0)
{ }

template< class T >
alekseev::Vector< T >::~Vector()
{
  delete [] data_;
}

template< class T >
alekseev::Vector< T >::Vector(const Vector & rhs)
{
  data_ = new T[rhs.size()];
  for (size_t i = 0; i < rhs.size(); ++i) {
    data_[i] = rhs.data_[i];
  }
  size_ = rhs.size();
  capacity_ = size_;
}

template< class T >
alekseev::Vector< T > & alekseev::Vector< T >::operator=(const Vector & rhs)
{
  if (this == std::addressof(rhs)) {
    return *this;
  }
  Vector cpy(rhs);
  swap(cpy);
  return *this;
}

template< class T >
alekseev::Vector< T >::Vector(Vector && rhs) noexcept:
  data_(rhs.data_),
  size_(rhs.size_),
  capacity_(rhs.capacity_)
{
  rhs.data_ = nullptr;
}

template< class T >
alekseev::Vector< T > & alekseev::Vector< T >::operator=(Vector && rhs) noexcept
{
  Vector cpy(std::move(rhs));
  swap(cpy);
  return *this;
}

template< class T >
alekseev::Vector< T >::Vector(size_t size, const T & value):
  Vector(size)
{
  for (size_t i = 0; i < size; i++) {
    data_[i] = value;
  }
}

template< class T >
alekseev::Vector< T >::Vector(std::initializer_list< T > init):
  Vector(init.size())
{
  size_t j = 0;
  for (auto i = init.begin(); i != init.end(); ++i) {
    data_[j++] = *i;
  }
}

template< class T >
bool alekseev::Vector< T >::empty() const noexcept
{
  return !size_;
}

template< class T >
size_t alekseev::Vector< T >::size() const noexcept
{
  return size_;
}

template< class T >
size_t alekseev::Vector< T >::capacity() const noexcept
{
  return capacity_;
}

template< class T >
T & alekseev::Vector< T >::operator[](size_t id) noexcept
{
  return const_cast< T & >((*static_cast< const Vector * >(this))[id]);
}

template< class T >
const T & alekseev::Vector< T >::operator[](size_t id) const noexcept
{
  return data_[id];
}

template< class T >
T & alekseev::Vector< T >::at(size_t id)
{
  const Vector * cthis = this;
  const T & cr = cthis->at(id);
  T & r = const_cast< T & >(cr);
  return r;
}

template< class T >
const T & alekseev::Vector< T >::at(size_t id) const
{
  if (id < size()) {
    return (*this)[id];
  }
  throw std::out_of_range("index out of range");
}

template< class T >
void alekseev::Vector< T >::swap(Vector & rhs) noexcept
{
  T * tmp = data_;
  data_ = rhs.data_;
  rhs.data_ = tmp;
  std::swap(size_, rhs.size_);
  std::swap(capacity_, rhs.capacity_);
}

template< class T >
void alekseev::Vector< T >::pushFront(const T & value)
{
  Vector v(size() + 1);
  v[0] = value;
  for (size_t i = 1; i < v.size(); ++i) {
    v[i] = (*this)[i - 1];
  }
  swap(v);
}

template< class T >
void alekseev::Vector< T >::pushBack(const T & value)
{
  if (size_ == capacity_) {
    T * new_data = nullptr;
    try {
      new_data = new T[capacity_ ? capacity_ * 2 : 1];
      capacity_ = capacity_ ? capacity_ * 2 : 1;
      for (size_t i = 0; i < size_; ++i) {
        new_data[i] = data_[i];
      }
      delete [] data_;
      data_ = new_data;
    } catch (...) {
      delete new_data;
      throw;
    }
  }
  data_[size_++] = value;
}

template< class T >
void alekseev::Vector< T >::push_back(const T & value)
{
  Vector v(size() + 1);
  for (size_t i = 0; i < size(); ++i) {
    v[i] = (*this)[i];
  }
  v[size()] = value;
  swap(v);
}

template< class T >
void alekseev::Vector< T >::popBack()
{
  if (size_ == 0) {
    throw std::logic_error("Logic error: vector is empty");
  }
  size_--;
}

template< class T >
bool alekseev::Vector< T >::operator==(const Vector & rhs) const
{
  bool isEqual = size() == rhs.size();
  for (size_t i = 0; isEqual && i < size(); ++i) {
    isEqual = (*this)[i] == rhs[i];
  }
  return isEqual;
}

template< class T >
void alekseev::Vector< T >::insert(size_t id, const T & value)
{
  Vector temp(size() + 1);
  for (size_t i = 0; i < size() + 1; ++i) {
    if (i < id) {
      temp[i] = (*this)[i];
    } else if (i == id) {
      temp[i] = value;
    } else {
      temp[i] = (*this)[i - 1];
    }
  }
  swap(temp);
}

template< class T >
void alekseev::Vector< T >::insert(size_t id, const Vector & rhs, size_t begin, size_t end)
{
  size_t len = end - begin;
  Vector temp(size() + len);

  for (size_t i = 0; i < size() + 1; ++i) {
    if (i < id) {
      temp[i] = (*this)[i];
    } else if (i == id) {
      for (size_t j = 0; j < len; ++j) {
        temp[i + j] = rhs[begin + j];
      }
    } else {
      temp[i + len] = (*this)[i - 1];
    }
  }
  swap(temp);
}

template< class T >
void alekseev::Vector< T >::erase(size_t id)
{
  Vector temp(size() - 1);
  for (size_t i = 0; i < size() - 1; ++i) {
    size_t j = i < id ? i : i + 1;
    temp[i] = (*this)[j];
  }
  swap(temp);
}

template< class T >
void alekseev::Vector< T >::erase(size_t begin, size_t end)
{
  size_t len = end - begin;
  Vector temp(size() - len);
  for (size_t i = 0; i < size() - len; ++i) {
    size_t j = i < begin ? i : i + len;
    temp[i] = (*this)[j];
  }
  swap(temp);
}

template< class T >
alekseev::Vector< T > alekseev::Vector< T >::operator+(const Vector & rhs) const
{
  Vector temp(size() + rhs.size());
  for (size_t i = 0; i < size(); ++i) {
    temp[i] = (*this)[i];
  }
  for (size_t i = 0; i < rhs.size(); ++i) {
    temp[i + size()] = rhs[i];
  }
  return temp;
}

template< class T >
alekseev::Vector< T > & alekseev::Vector< T >::operator+=(const Vector & rhs)
{
  Vector temp = *this + rhs;
  swap(temp);
  return *this;
}

template< class T >
template< class Less >
void alekseev::Vector< T >::bubbleSort(Less less)
{
  if (size_ <= 1) {
    return;
  }
  bool swapped = true;
  Vector temp = *this;
  while (swapped) {
    swapped = false;
    for (size_t i = 0; i < size() - 1; ++i) {
      if (less(temp[i + 1], temp[i])) {
        swapped = true;
        T t = temp[i];
        temp[i] = temp[i + 1];
        temp[i + 1] = t;
      }
    }
  }
  swap(temp);
}

template< class T >
void alekseev::Vector< T >::resize(size_t new_capacity)
{
  if (new_capacity <= size()) {
    return;
  }
  T * temp = new T[new_capacity];
  try {
    for (size_t i = 0; i < size(); ++i) {
      temp[i] = data_[i];
    }
  } catch (...) {
    delete[] temp;
  }
  delete [] data_;
  data_ = temp;
  capacity_ = new_capacity;
}

template< class T >
alekseev::Vector< T >::Vector(size_t size):
  data_(size ? new T[size] : nullptr),
  size_(size),
  capacity_(size)
{ }

#endif
