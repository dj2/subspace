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

#include <functional>  // TODO: Replace std::function with something.

#include "traits/iter/iterator_defn.h"
#include "traits/iter/sized_iterator.h"

// IteratorBase provided functions are implemented in this file, so that they
// can be easily included by library users, but don't have to be included in
// every library header that returns an IteratorBase.

namespace sus::traits::iter {

struct IteratorEnd {};
extern const IteratorEnd iterator_end;

/// An adaptor for range-based for loops.
template <class IteratorBase>
class IteratorLoop {
  using Item = typename IteratorBase::Item;

 public:
  IteratorLoop(IteratorBase& iter) : iter_(iter), item_(iter_.next()) {}

  inline bool operator==(const IteratorEnd&) const { return item_.is_nome(); }
  inline bool operator!=(const IteratorEnd&) const { return item_.is_some(); }
  inline void operator++() & { item_ = iter_.next(); }
  inline Item operator*() & { return item_.take().unwrap(); }

 private:
  /* TODO: NonNull<IteratorBase<Item>> */ IteratorBase& iter_;
  Option<Item> item_;
};

template <class Item>
IteratorLoop<IteratorBase<Item>> IteratorBase<Item>::begin() & noexcept {
  return {*this};
}

template <class Item>
IteratorEnd IteratorBase<Item>::end() & noexcept {
  return iterator_end;
}

template <class Item>
bool IteratorBase<Item>::all(std::function<bool(Item)> f) noexcept {
  Option<Item> item = next();
  while (item.is_some()) {
    // Safety: `item_` was checked to hold Some already.
    Item i = item.take().unwrap_unchecked(unsafe_fn);
    if (!f(static_cast<decltype(i)&&>(i))) return false;
    item = next();
  }
  return true;
}

template <class Item>
bool IteratorBase<Item>::any(std::function<bool(Item)> f) noexcept {
  Option<Item> item = next();
  while (item.is_some()) {
    // Safety: `item_` was checked to hold Some already.
    Item i = item.take().unwrap_unchecked(unsafe_fn);
    if (f(static_cast<decltype(i)&&>(i))) return true;
    item = next();
  }
  return false;
}

template <class Item>
size_t IteratorBase<Item>::count() noexcept {
  size_t c = 0;
  Option<Item> item = next();
  while (item.is_some()) {
    c += 1;
    item = next();
  }
  return c;
}

template <class I>
Iterator<Filter<typename I::Item, sizeof(I), alignof(I)>> Iterator<I>::filter(
    std::function<bool(const std::remove_reference_t<typename I::Item>&)>
        pred) && noexcept {
  return {static_cast<decltype(pred)&&>(pred),
          make_sized_iterator(static_cast<I&&>(*this))};
}

}  // namespace sus::traits::iter