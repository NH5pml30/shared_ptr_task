#ifndef WEAK_PTR_H_
#define WEAK_PTR_H_

#include <memory>
#include "shared_ptr.h"

template<typename T>
struct weak_ptr
{
public:
  weak_ptr() noexcept = default;

  template<typename Y>
  weak_ptr(const shared_ptr<Y> &other) noexcept;

  weak_ptr(const weak_ptr &other) noexcept;
  template<typename Y>
  weak_ptr(const weak_ptr<Y> &other) noexcept;
  weak_ptr(weak_ptr &&other) noexcept;
  template<typename Y>
  weak_ptr(weak_ptr<Y> &&other) noexcept;

  weak_ptr & operator=(const weak_ptr &other) noexcept;
  template<typename Y>
  weak_ptr & operator=(const weak_ptr<Y> &other) noexcept;
  weak_ptr & operator=(weak_ptr &&other) noexcept;
  template<typename Y>
  weak_ptr & operator=(weak_ptr<Y> &&other) noexcept;

  ~weak_ptr();

  void reset() noexcept;

  shared_ptr<T> lock() const noexcept;
private:
  control_block *cblock = nullptr;
  T *ptr = nullptr;

  weak_ptr(control_block *cblock, T *ptr) noexcept;

  template<typename Y, typename U>
  friend void swap(weak_ptr<Y> &left, weak_ptr<U> &right) noexcept;
};

template<typename T>
weak_ptr<T>::weak_ptr(control_block *cblock, T *ptr) noexcept : cblock(cblock), ptr(ptr)
{
  if (cblock != nullptr)
  {
    cblock->add_weak();
  }
}

template<typename T>
template<typename Y>
weak_ptr<T>::weak_ptr(const shared_ptr<Y> &other) noexcept : weak_ptr(other.cblock, other.ptr)
{
}

template<typename T>
weak_ptr<T>::weak_ptr(const weak_ptr &other) noexcept : weak_ptr(other.cblock, other.ptr)
{
}

template<typename T>
template<typename Y>
weak_ptr<T>::weak_ptr(const weak_ptr<Y> &other) noexcept : weak_ptr(other.cblock, other.ptr)
{
}

template<typename T>
weak_ptr<T>::weak_ptr(weak_ptr &&other) noexcept : cblock(other.cblock), ptr(other.ptr)
{
  other.cblock = nullptr;
  other.ptr = nullptr;
}

template<typename T>
template<typename Y>
weak_ptr<T>::weak_ptr(weak_ptr<Y> &&other) noexcept : cblock(other.cblock), ptr(other.ptr)
{
  other.cblock = nullptr;
  other.ptr = nullptr;
}

template<typename T>
weak_ptr<T> & weak_ptr<T>::operator=(const weak_ptr &other) noexcept
{
  return operator=<T>(other);
}

template<typename T, typename Y>
void swap(weak_ptr<T> &left, weak_ptr<Y> &right) noexcept
{
  std::swap(left.cblock, right.cblock);
  std::swap(left.ptr, right.ptr);
}


template<typename T>
template<typename Y>
weak_ptr<T> & weak_ptr<T>::operator=(const weak_ptr<Y> &other) noexcept
{
  weak_ptr<T> copy(other);
  swap(*this, copy);
  return *this;
}

template<typename T>
weak_ptr<T> & weak_ptr<T>::operator=(weak_ptr &&other) noexcept
{
  return operator=<T>(std::move(other));
}

template<typename T>
template<typename Y>
weak_ptr<T> & weak_ptr<T>::operator=(weak_ptr<Y> &&other) noexcept
{
  if (&other != this)
  {
    weak_ptr<T> empty;
    swap(*this, empty);
    swap(other, *this);
  }
  return *this;
}

template<typename T>
weak_ptr<T>::~weak_ptr()
{
  if (cblock != nullptr)
  {
    cblock->del_weak();
  }
}

template<typename T>
void weak_ptr<T>::reset() noexcept
{
  weak_ptr<T> empty;
  swap(*this, empty);
  return *this;
}

template<typename T>
shared_ptr<T> weak_ptr<T>::lock() const noexcept
{
  if (cblock == nullptr || cblock->ref_count() == 0)
    return shared_ptr<T>();
  return shared_ptr<T>(cblock, ptr);
}

#endif /* WEAK_PTR_H_ */
