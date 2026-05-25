#ifndef VECTOR_H
#define VECTOR_H

#include <stdexcept>
#include <initializer_list>
#include <iostream>

namespace alekseev {
  template< class T >
  struct Vector {
    ~Vector();
    Vector(const Vector< T > & rhs);
    Vector< T > & operator=(const Vector< T > & rhs);
    Vector(Vector< T > && rhs) noexcept;
    Vector< T > & operator=(Vector< T > && rhs) noexcept;

    Vector();
    explicit Vector(std::initializer_list< T > init);
    Vector(size_t size, const T & value);

    bool isEmpty() const noexcept;
    size_t getSize() const noexcept;
    size_t getCapacity() const noexcept;

    T & operator[](size_t id) noexcept;
    const T & operator[](size_t id) const noexcept;
    T & at(size_t id);
    const T & at(size_t id) const;
    void swap(Vector< T > & rhs) noexcept;
    void pushFront(const T & value);
    void pushBack(const T &);
    void push_back(const T & value);
    void popBack();
    bool operator==(const Vector< T > & rhs) const;

    void insert(size_t id, const T & value);
    void insert(size_t id, const Vector< T > & rhs, size_t begin, size_t end);
    void erase(size_t id);
    void erase(size_t begin, size_t end);

    void bubbleSort(bool (* less)(T, T));
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
{
}

template< class T >
alekseev::Vector< T >::~Vector()
{
  delete [] data_;
}

template< class T >
alekseev::Vector< T >::Vector(const Vector< T > & rhs)
{
  data_ = new T[rhs.getSize()];
  for (size_t i = 0; i < rhs.getSize(); ++i) {
    data_[i] = rhs.data_[i];
  }
  size_ = rhs.getSize();
  capacity_ = size_;
}

template< class T >
alekseev::Vector< T > & alekseev::Vector< T >::operator=(const Vector< T > & rhs)
{
  if (this == std::addressof(rhs)) {
    return *this;
  }
  Vector< T > cpy(rhs);
  swap(cpy);
  return *this;
}

template< class T >
alekseev::Vector< T >::Vector(Vector< T > && rhs) noexcept:
  data_(rhs.data_),
  size_(rhs.size_),
  capacity_(rhs.capacity_)
{
  rhs.data_ = nullptr;
}

template< class T >
alekseev::Vector< T > & alekseev::Vector< T >::operator=(Vector< T > && rhs) noexcept
{
  Vector< T > cpy(std::move(rhs));
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
bool alekseev::Vector< T >::isEmpty() const noexcept
{
  return !size_;
}

template< class T >
size_t alekseev::Vector< T >::getSize() const noexcept
{
  return size_;
}

template< class T >
size_t alekseev::Vector< T >::getCapacity() const noexcept
{
  return capacity_;
}

template< class T >
T & alekseev::Vector< T >::operator[](size_t id) noexcept
{
  return const_cast< T & >((*static_cast< const Vector< T > * >(this))[id]);
}

template< class T >
const T & alekseev::Vector< T >::operator[](size_t id) const noexcept
{
  return data_[id];
}

template< class T >
T & alekseev::Vector< T >::at(size_t id)
{
  const Vector< T > * cthis = this;
  const T & cr = cthis->at(id);
  T & r = const_cast< T & >(cr);
  return r;
}

template< class T >
const T & alekseev::Vector< T >::at(size_t id) const
{
  if (id < getSize()) {
    return (*this)[id];
  }
  throw std::out_of_range("index out of range");
}

template< class T >
void alekseev::Vector< T >::swap(Vector< T > & rhs) noexcept
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
  Vector< T > v(getSize() + 1);
  v[0] = value;
  for (size_t i = 1; i < v.getSize(); ++i) {
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
  Vector< T > v(getSize() + 1);
  for (size_t i = 0; i < getSize(); ++i) {
    v[i] = (*this)[i];
  }
  v[getSize()] = value;
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
bool alekseev::Vector< T >::operator==(const Vector< T > & rhs) const
{
  bool isEqual = getSize() == rhs.getSize();
  for (size_t i = 0; isEqual && i < getSize(); ++i) {
    isEqual = (*this)[i] == rhs[i];
  }
  return isEqual;
}

template< class T >
void alekseev::Vector< T >::insert(size_t id, const T & value)
{
  Vector< T > temp(getSize() + 1);
  for (size_t i = 0; i < getSize() + 1; ++i) {
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
void alekseev::Vector< T >::insert(size_t id, const Vector< T > & rhs, size_t begin, size_t end)
{
  size_t len = end - begin;
  Vector< T > temp(getSize() + len);

  for (size_t i = 0; i < getSize() + 1; ++i) {
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
  Vector< T > temp(getSize() - 1);
  for (size_t i = 0; i < getSize() - 1; ++i) {
    size_t j = i < id ? i : i + 1;
    temp[i] = (*this)[j];
  }
  swap(temp);
}

template< class T >
void alekseev::Vector< T >::erase(size_t begin, size_t end)
{
  size_t len = end - begin;
  Vector< T > temp(getSize() - len);
  for (size_t i = 0; i < getSize() - len; ++i) {
    size_t j = i < begin ? i : i + len;
    temp[i] = (*this)[j];
  }
  swap(temp);
}

template< class T >
void alekseev::Vector< T >::bubbleSort(bool (* less)(T, T))
{
  if (size_ <= 1) {
    return;
  }
  bool swapped = true;
  Vector< T > temp = *this;
  while (swapped) {
    swapped = false;
    for (size_t i = 0; i < getSize() - 1; ++i) {
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
  T * temp = new T[new_capacity];
  for (size_t i = 0; i < getSize(); ++i) {
    temp[i] = data_[i];
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
{
}

#endif
