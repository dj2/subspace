// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>

#include <type_traits>

#include "iter/iterator_defn.h"
#include "mem/move.h"
#include "mem/mref.h"
#include "mem/nonnull.h"
#include "num/unsigned_integer.h"

namespace sus::containers {

template <class T>
class Slice;

template <class Item>
struct SliceIter : public ::sus::iter::IteratorBase<const Item&> {
 public:
  static constexpr auto with(const Slice<Item>& slice) noexcept {
    return ::sus::iter::Iterator<SliceIter>(slice);
  }

  Option<const Item&> next() noexcept final {
    if (ptr_ == end_) [[unlikely]]
      return Option<const Item&>::none();
    // SAFETY: Since end_ > ptr_, which is checked in the constructor, ptr_ + 1
    // will never be null.
    return Option<const Item&>::some(
        *::sus::mem::replace(mref(ptr_), ptr_ + 1_usize));
  }

 protected:
  constexpr SliceIter(const Slice<Item>& slice) noexcept
      : ptr_(slice.as_ptr()), end_(ptr_ + slice.len()) {
    check(end_ > ptr_ || !end_);  // end_ may wrap around to 0, but not past 0.
  }

 private:
  const T* ptr_;
  const T* end_;
};

template <class Item>
struct SliceIterMut : public ::sus::iter::IteratorBase<const Item&> {
 public:
  static constexpr auto with(Slice<Item>& slice) noexcept {
    return ::sus::iter::Iterator<SliceIterMut>(slice);
  }

  Option<Item&> next() noexcept final {
    if (ptr_ == end_) [[unlikely]]
      return Option<Item&>::none();
    // SAFETY: Since end_ > ptr_, which is checked in the constructor, ptr_ + 1
    // will never be null.
    return Option<Item&>::some(
        *::sus::mem::replace(mref(ptr_), ptr_ + 1_usize));
  }

 protected:
  constexpr SliceIterMut(Slice<Item>& slice) noexcept
      : ptr_(slice.as_ptr_ptr()), end_(ptr_ + slice.len()) {
    check(end_ > ptr_ || !end_);  // end_ may wrap around to 0, but not past 0.
  }

 private:
  T* ptr_;
  T* end_;
};

}  // namespace sus::containers