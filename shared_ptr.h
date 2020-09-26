#pragma once

#include "control_block.h"
#include <memory>

template<typename T>
struct weak_ptr;

template<typename T>
struct shared_ptr
{
public:
  shared_ptr() noexcept = default;

  template<typename Y>
  explicit shared_ptr(Y *ptr);
  explicit shared_ptr(std::nullptr_t);

  template<typename Y, class Deleter>
  shared_ptr(Y *ptr, Deleter d);
  template<class Deleter>
  shared_ptr(std::nullptr_t, Deleter d);

  shared_ptr(const shared_ptr &other) noexcept;
  template<typename Y>
  shared_ptr(const shared_ptr<Y> &other) noexcept;
  shared_ptr(shared_ptr &&other) noexcept;
  template<typename Y>
  shared_ptr(shared_ptr<Y> &&other) noexcept;

  template<typename Y>
  explicit shared_ptr(const shared_ptr<Y> &other, T *ptr) noexcept;

  shared_ptr & operator=(const shared_ptr &other) noexcept;
  template<typename Y>
  shared_ptr & operator=(const shared_ptr<Y> &other) noexcept;
  shared_ptr & operator=(shared_ptr &&other) noexcept;
  template<typename Y>
  shared_ptr & operator=(shared_ptr<Y> &&other) noexcept;

  ~shared_ptr();

  void reset() noexcept;
  template<typename Y>
  void reset(Y *ptr);
  template<typename Y, class Deleter>
  void reset(Y *ptr, Deleter d);

  T * get() const noexcept;
  T & operator*() const noexcept;
  T * operator->() const noexcept;

  explicit operator bool() const noexcept;

  size_t use_count() const noexcept;

private:
  control_block *cblock = nullptr;
  T *ptr = nullptr;

  template<typename Y>
  shared_ptr(control_block *cblock, Y *ptr) noexcept;
  template<typename Y>
  shared_ptr(const shared_ptr<Y> &other, int) noexcept;
  template<typename Y>
  shared_ptr(shared_ptr<Y> &&other, int) noexcept;

  template<typename Y>
  shared_ptr & swap(shared_ptr<Y> &other);

  template<typename Y>
  friend class shared_ptr;

  friend class weak_ptr<T>;

  template<typename Y, typename ...Args>
  friend shared_ptr<Y> make_shared(Args&&... args);
};

template<typename T, typename U>
bool operator==(const shared_ptr<T> &left, const shared_ptr<U> &right)
{
  return left.get() == right.get();
}

template<typename T>
bool operator==(const shared_ptr<T> &left, std::nullptr_t)
{
  return left.get() == nullptr;
}

template<typename T>
bool operator==(std::nullptr_t, const shared_ptr<T> &right)
{
  return right.get() == nullptr;
}

template<typename T, typename U>
bool operator!=(const shared_ptr<T> &left, const shared_ptr<U> &right)
{
  return !operator==(left, right);
}

template<typename T>
bool operator!=(const shared_ptr<T> &left, std::nullptr_t)
{
  return !operator==(left, nullptr);
}

template<typename T>
bool operator!=(std::nullptr_t, const shared_ptr<T> &right)
{
  return !operator==(nullptr, right);
}

template<typename T>
template<typename Y>
shared_ptr<T>::shared_ptr(control_block *cblock, Y *ptr) noexcept : cblock(cblock), ptr(ptr)
{
  if (cblock != nullptr)
  {
    cblock->add_ref();
  }
}

template<typename T>
template<typename Y>
shared_ptr<T>::shared_ptr(const shared_ptr<Y> &other, int) noexcept : shared_ptr(other.cblock, other.ptr)
{
}

template<typename T>
template<typename Y>
shared_ptr<T>::shared_ptr(shared_ptr<Y> &&other, int) noexcept : cblock(other.cblock), ptr(other.ptr)
{
  other.cblock = nullptr;
  other.ptr = nullptr;
}

template<typename T>
template<typename Y>
shared_ptr<T>::shared_ptr(Y *ptr) : shared_ptr(ptr, std::default_delete<Y>())
{
}

template<typename T>
shared_ptr<T>::shared_ptr(std::nullptr_t) {}

template<typename T>
shared_ptr<T>::shared_ptr(const shared_ptr &other) noexcept : shared_ptr(other, 0)
{
}

template<typename T>
template<typename Y>
shared_ptr<T>::shared_ptr(const shared_ptr<Y> &other) noexcept : shared_ptr(other, 0)
{
}

template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr &&other) noexcept : shared_ptr(std::move(other), 0)
{
}

template<typename T>
template<typename Y>
shared_ptr<T>::shared_ptr(shared_ptr<Y> &&other) noexcept : shared_ptr(std::move(other), 0)
{
}

template<typename T>
template<typename Y, class Deleter>
shared_ptr<T>::shared_ptr(Y *ptr, Deleter d) : ptr(ptr) {
  try
  {
    cblock = new regular_control_block<Y, Deleter>(ptr, d);
  }
  catch (const std::exception &e)
  {
    delete ptr;
    throw;
  }
}

template<typename T>
template<class Deleter>
shared_ptr<T>::shared_ptr(std::nullptr_t, Deleter d) {}

template<typename T>
template<typename Y>
shared_ptr<T>::shared_ptr(const shared_ptr<Y> &other, T *ptr) noexcept : shared_ptr(other.cblock, ptr)
{
}

template<typename T>
template<typename Y>
shared_ptr<T> & shared_ptr<T>::swap(shared_ptr<Y> &other)
{
  std::swap(ptr, other.ptr);
  std::swap(cblock, other.cblock);
  return *this;
}

template<typename T>
shared_ptr<T> & shared_ptr<T>::operator=(const shared_ptr<T> &other) noexcept
{
  return operator=<T>(other);
}
template<typename T>
shared_ptr<T> & shared_ptr<T>::operator=(shared_ptr<T> &&other) noexcept
{
  return operator=<T>(std::move(other));
}

template<typename T>
template<typename Y>
shared_ptr<T> & shared_ptr<T>::operator=(const shared_ptr<Y> &other) noexcept
{
  shared_ptr<Y>(other).swap(*this);
  return *this;
}

template<typename T>
template<typename Y>
shared_ptr<T> & shared_ptr<T>::operator=(shared_ptr<Y> &&other) noexcept
{
  if (&other != this)
  {
    shared_ptr<T>().swap(*this);
    return swap(other);
  }
  return *this;
}

template<typename T>
shared_ptr<T>::~shared_ptr()
{
  if (cblock != nullptr)
  {
    cblock->del_ref();
    if (cblock->ref_count() == 0)
    {
      cblock->delete_object();
      control_block::delete_this(cblock);
    }
  }
}

template<typename T>
void shared_ptr<T>::reset() noexcept
{
  shared_ptr<T>().swap(*this);
}

template<typename T>
template<typename Y>
void shared_ptr<T>::reset(Y *ptr)
{
  shared_ptr<T>(ptr).swap(*this);
}

template<typename T>
template<typename Y, class Deleter>
void shared_ptr<T>::reset(Y *ptr, Deleter d)
{
  shared_ptr<T>(ptr, d).swap(*this);
}

template<typename T>
T * shared_ptr<T>::get() const noexcept
{
  return ptr;
}

template<typename T>
T & shared_ptr<T>::operator*() const noexcept
{
  return *ptr;
}

template<typename T>
T * shared_ptr<T>::operator->() const noexcept
{
  return ptr;
}

template<typename T>
shared_ptr<T>::operator bool() const noexcept
{
  return ptr;
}

template<typename T>
size_t shared_ptr<T>::use_count() const noexcept
{
  return cblock ? cblock->ref_count() : 0;
}

template<typename T, typename ...Args>
shared_ptr<T> make_shared(Args&&... args)
{
  inplace_control_block<T> *cblock = new inplace_control_block<T>(std::forward<Args>(args)...);
  shared_ptr<T> res;
  res.cblock = cblock;
  res.ptr = reinterpret_cast<T *>(&cblock->stg);
  return res;
}
