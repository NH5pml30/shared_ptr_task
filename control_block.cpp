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
  if (n_shared_refs == 0)
  {
    delete_object();
    del_weak();
  }
}

void control_block::del_weak() noexcept
{
  n_weak_refs--;
  if (n_weak_refs == 0 && n_shared_refs == 0)
  {
    delete this;
  }
}

size_t control_block::ref_count() const noexcept
{
  return n_shared_refs;
}
