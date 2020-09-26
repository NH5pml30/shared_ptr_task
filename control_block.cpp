#include "control_block.h"

void control_block::add_ref() noexcept
{
  n_shared_refs++;
}

void control_block::add_weak() noexcept
{
  n_weak_refs++;
}

void control_block::del_ref() noexcept
{
  n_shared_refs--;
}

void control_block::del_weak() noexcept
{
  n_weak_refs--;
}

size_t control_block::ref_count() const noexcept
{
  return n_shared_refs;
}

size_t control_block::weak_ref_count() const noexcept
{
  return n_weak_refs;
}

void control_block::delete_this(control_block *cb) noexcept
{
  if (cb != nullptr && cb->n_weak_refs == 0)
  {
    delete cb;
  }
}
