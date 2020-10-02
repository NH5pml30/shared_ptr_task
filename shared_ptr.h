#ifndef SHARED_PTR_H_
#define SHARED_PTR_H_

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

  shared_ptr(std::nullptr_t) noexcept;


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
  shared_ptr(const shared_ptr<Y> &other, T *ptr) noexcept;


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

  template<typename Y, typename U>
  friend void swap(shared_ptr<Y> &left, shared_ptr<U> &right) noexcept;

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
shared_ptr<T>::shared_ptr(Y *ptr) : shared_ptr(ptr, std::default_delete<Y>())
{
}

template<typename T>
shared_ptr<T>::shared_ptr(std::nullptr_t) noexcept {}

template<typename T>
shared_ptr<T>::shared_ptr(const shared_ptr &other) noexcept : shared_ptr(other.cblock, other.ptr)
{
}

template<typename T>
template<typename Y>
shared_ptr<T>::shared_ptr(const shared_ptr<Y> &other) noexcept : shared_ptr(other.cblock, other.ptr)
{
}

template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr &&other) noexcept : cblock(other.cblock), ptr(other.ptr)
{
  other.cblock = nullptr;
  other.ptr = nullptr;
}

template<typename T>
template<typename Y>
shared_ptr<T>::shared_ptr(shared_ptr<Y> &&other) noexcept : cblock(other.cblock), ptr(other.ptr)
{
  other.cblock = nullptr;
  other.ptr = nullptr;
}

template<typename T>
template<typename Y, class Deleter>
shared_ptr<T>::shared_ptr(Y *ptr, Deleter d) : ptr(ptr) {
  try
  {
    cblock = new regular_control_block<Y, Deleter>(ptr, std::move(d));
  }
  catch (const std::exception &e)
  {
    delete ptr;
    throw;
  }
}

template<typename T>
template<class Deleter>
shared_ptr<T>::shared_ptr(std::nullptr_t, Deleter d) : shared_ptr((T *)nullptr, std::move(d)) {}

template<typename T>
template<typename Y>
shared_ptr<T>::shared_ptr(const shared_ptr<Y> &other, T *ptr) noexcept : shared_ptr(other.cblock, ptr)
{
}

template<typename T, typename Y>
void swap(shared_ptr<T> &left, shared_ptr<Y> &right) noexcept
{
  std::swap(left.ptr, right.ptr);
  std::swap(left.cblock, right.cblock);
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
  shared_ptr<T> copy(other);
  swap(copy, *this);
  return *this;
}

template<typename T>
template<typename Y>
shared_ptr<T> & shared_ptr<T>::operator=(shared_ptr<Y> &&other) noexcept
{
  if (cblock != other.cblock)
  {
    shared_ptr<T> empty;
    swap(*this, empty);
    swap(other, *this);
  }
  return *this;
}

template<typename T>
shared_ptr<T>::~shared_ptr()
{
  if (cblock != nullptr)
  {
    cblock->del_ref();
  }
}

template<typename T>
void shared_ptr<T>::reset() noexcept
{
  shared_ptr<T> empty;
  swap(*this, empty);
}

template<typename T>
template<typename Y>
void shared_ptr<T>::reset(Y *ptr)
{
  shared_ptr<T> other(ptr);
  swap(*this, other);
}

template<typename T>
template<typename Y, class Deleter>
void shared_ptr<T>::reset(Y *ptr, Deleter d)
{
  shared_ptr<T> other(ptr, std::move(d));
  swap(*this, other);
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

#endif /* SHARED_PTR_H_ */
