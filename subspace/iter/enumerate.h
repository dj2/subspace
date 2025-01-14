// Copyright 2023 Google LLC
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

#include "subspace/fn/fn_box_defn.h"
#include "subspace/iter/iterator_defn.h"
#include "subspace/iter/sized_iterator.h"
#include "subspace/mem/move.h"
#include "subspace/mem/relocate.h"
#include "subspace/tuple/tuple.h"

namespace sus::iter {

/// An iterator that yields the current count and the element during iteration.
///
/// This class is created by the `enumerate()` method on `IteratorBase`. See its
/// documentation for more.
template <class InnerSizedIter>
class [[nodiscard]] [[sus_trivial_abi]] Enumerate final
    : public IteratorBase<Enumerate<InnerSizedIter>,
                          ::sus::Tuple<usize, typename InnerSizedIter::Item>> {
 public:
  using Item = ::sus::Tuple<usize, typename InnerSizedIter::Item>;

  static Enumerate with(InnerSizedIter&& next_iter) noexcept {
    return Enumerate(::sus::move(next_iter));
  }

  // sus::iter::Iterator trait.
  Option<Item> next() noexcept {
    Option<typename InnerSizedIter::Item> item = next_iter_.next();
    if (item.is_none()) {
      return sus::none();
    } else {
      usize count = count_;
      count_ += 1;
      return sus::some(sus::tuple(
          count, sus::move(item).unwrap_unchecked(::sus::marker::unsafe_fn)));
    }
  }

  // sus::iter::DoubleEndedIterator trait.
  Option<Item> next_back() noexcept
    requires(InnerSizedIter::DoubleEnded && InnerSizedIter::ExactSize)
  {
    Option<typename InnerSizedIter::Item> item = next_iter_.next_back();
    if (item.is_none()) {
      return sus::none();
    } else {
      usize len = next_iter_.exact_size_hint();
      // Can safely add, `ExactSizeIterator` promises that the number of
      // elements fits into a `usize`.
      return sus::some(sus::tuple(
          count_ + len,
          sus::move(item).unwrap_unchecked(::sus::marker::unsafe_fn)));
    }
  }

  // sus::iter::ExactSizeIterator trait.
  usize exact_size_hint() const noexcept
    requires(InnerSizedIter::ExactSize)
  {
    return next_iter_.exact_size_hint();
  }

  // TODO: Implement nth(), nth_back(), etc...

 private:
  Enumerate(InnerSizedIter&& next_iter) : next_iter_(::sus::move(next_iter)) {}

  usize count_ = 0u;
  InnerSizedIter next_iter_;

  // The InnerSizedIter is trivially relocatable.
  sus_class_trivially_relocatable(::sus::marker::unsafe_fn,
                                  decltype(next_iter_));
};

}  // namespace sus::iter
