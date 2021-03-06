#ifndef CONTROL_BLOCK_H_
#define CONTROL_BLOCK_H_

#include <utility>
#include <type_traits>
#include <memory>

struct control_block
{
  control_block() noexcept = default;

  void add_ref() noexcept;
  void add_weak() noexcept;
  void del_ref() noexcept;
  void del_weak() noexcept;

  size_t ref_count() const noexcept;

  virtual void delete_object() noexcept = 0;
protected:
  virtual ~control_block() = default;

private:
  size_t n_shared_refs = 1, n_weak_refs = 1;
};

template<typename T, class Deleter>
struct regular_control_block final : control_block, Deleter
{
  explicit regular_control_block(T *ptr, Deleter d);
  void delete_object() noexcept override;
private:
  T *ptr;
};

template<typename T>
class shared_ptr;

template<typename T>
struct inplace_control_block final : control_block
{
  template<typename ...Args>
  explicit inplace_control_block(Args&&... args);

  void delete_object() noexcept override;

  typename std::aligned_storage<sizeof(T), alignof(T)>::type stg;
};

template<typename T, class Deleter>
regular_control_block<T, Deleter>::regular_control_block(T * ptr, Deleter d) : Deleter(std::move(d)), ptr(ptr)
{
}

template<typename T, class Deleter>
void regular_control_block<T, Deleter>::delete_object() noexcept
{
  Deleter::operator()(ptr);
}

template<typename T>
template<typename ...Args>
inplace_control_block<T>::inplace_control_block(Args&&... args)
{
  new(&stg) T(std::forward<Args>(args)...);
}

template<typename T>
void inplace_control_block<T>::delete_object() noexcept
{
  reinterpret_cast<T *>(&stg)->~T();
}

#endif /* CONTROL_BLOCK_H_ */
