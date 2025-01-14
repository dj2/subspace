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

#include "subspace/iter/iterator_concept.h"
#include "subspace/mem/relocate.h"
#include "subspace/mem/size_of.h"
#include "subspace/option/option.h"
#include "subspace/ptr/copy.h"
// Doesn't include iterator_defn.h because it's included from there.

namespace sus::iter {

template <class ItemT, size_t SubclassSize, size_t SubclassAlign,
          bool DoubleEndedB, bool ExactSizeB>
struct [[sus_trivial_abi]] SizedIterator final {
  using Item = ItemT;
  static constexpr bool DoubleEnded = DoubleEndedB;
  static constexpr bool ExactSize = ExactSizeB;

  constexpr SizedIterator(void (*destroy)(char& sized),
                          Option<Item> (*next)(char& sized),
                          Option<Item> (*next_back)(char& sized),
                          usize (*exact_size_hint)(const char& sized))
      : destroy_(destroy),
        next_(next),
        next_back_(next_back),
        exact_size_hint_(exact_size_hint) {}

  SizedIterator(SizedIterator&& o) noexcept
      : destroy_(::sus::mem::replace(mref(o.destroy_), nullptr)),
        next_(::sus::mem::replace(mref(o.next_), nullptr)),
        next_back_(::sus::mem::replace(mref(o.next_back_), nullptr)),
        exact_size_hint_(
            ::sus::mem::replace(mref(o.exact_size_hint_), nullptr)) {
    ::sus::ptr::copy_nonoverlapping(::sus::marker::unsafe_fn, o.sized_, sized_,
                                    SubclassSize);
  }
  SizedIterator& operator=(SizedIterator&& o) noexcept {
    if (destroy_) destroy_(*sized_);
    destroy_ = ::sus::mem::replace(mref(o.destroy_), nullptr);
    next_ = ::sus::mem::replace(mref(o.next_), nullptr);
    next_back_ = ::sus::mem::replace(mref(o.next_back_), nullptr);
    exact_size_hint_ = ::sus::mem::replace(mref(o.exact_size_hint_), nullptr);
    ::sus::ptr::copy_nonoverlapping(::sus::marker::unsafe_fn, o.sized_, sized_,
                                    SubclassSize);
  }

  ~SizedIterator() noexcept {
    if (destroy_) destroy_(*sized_);
  }

  Option<Item> next() noexcept { return next_(*sized_); }
  Option<Item> next_back() noexcept
    requires(DoubleEnded)
  {
    return next_back_(*sized_);
  }
  usize exact_size_hint() const noexcept
    requires(ExactSize)
  {
    return exact_size_hint_(*sized_);
  }

  char* as_mut_ptr() noexcept { return sized_; }

 private:
  alignas(SubclassAlign) char sized_[SubclassSize];
  void (*destroy_)(char& sized);
  Option<Item> (*next_)(char& sized);
  // TODO: We could remove this field with a nested struct + template
  // specialization when DoubleEnded is false.
  Option<Item> (*next_back_)(char& sized);
  // TODO: We could remove this field with a nested struct + template
  // specialization when ExactSize is false.
  usize (*exact_size_hint_)(const char& sized);

  sus_class_trivially_relocatable(::sus::marker::unsafe_fn, decltype(sized_),
                                  decltype(destroy_), decltype(next_back_));
};

template <class Iter>
struct SizedIteratorType {
  using type =
      SizedIterator<typename Iter::Item, ::sus::mem::size_of<Iter>(),
                    alignof(Iter),
                    ::sus::iter::DoubleEndedIterator<Iter, typename Iter::Item>,
                    ::sus::iter::ExactSizeIterator<Iter, typename Iter::Item>>;
};

/// Make a SizedIterator which wraps a trivially relocatable iterator and erases
/// its type.
///
/// This type may only be used when the IteratorSubclass can be trivially
/// relocated. It stores the SubclassIterator directly into the SizedIterator,
/// erasing its type but remaining trivially relocatable.
template <::sus::mem::Move Iter, int&..., class Item = typename Iter::Item>
inline SizedIteratorType<Iter>::type make_sized_iterator(Iter&& iter)
  requires(::sus::iter::Iterator<Iter, Item> &&
           ::sus::mem::relocate_by_memcpy<Iter>)
{
  // IteratorBase also checks this. It's needed for correctness of the casts
  // here.
  static_assert(std::is_final_v<Iter>);

  void (*destroy)(char& sized) = [](char& sized) {
    reinterpret_cast<Iter&>(sized).~Iter();
  };
  Option<Item> (*next)(char& sized) = [](char& sized) {
    return reinterpret_cast<Iter&>(sized).next();
  };
  Option<Item> (*next_back)(char& sized);
  if constexpr (SizedIteratorType<Iter>::type::DoubleEnded) {
    next_back = [](char& sized) {
      return reinterpret_cast<Iter&>(sized).next_back();
    };
  } else {
    next_back = nullptr;
  }
  usize (*exact_size_hint)(const char& sized);
  if constexpr (SizedIteratorType<Iter>::type::ExactSize) {
    exact_size_hint = [](const char& sized) {
      return reinterpret_cast<const Iter&>(sized).exact_size_hint();
    };
  } else {
    exact_size_hint = nullptr;
  }

  auto it = typename SizedIteratorType<Iter>::type(destroy, next, next_back,
                                                   exact_size_hint);
  new (it.as_mut_ptr()) Iter(::sus::move(iter));
  return it;
}

}  // namespace sus::iter
