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

#include <concepts>

#include "assertions/check.h"
#include "construct/into.h"
#include "iter/iterator_defn.h"
#include "mem/never_value.h"
#include "num/unsigned_integer.h"
#include "option/option.h"

namespace sus::containers {

/// A dynamically-sized view into a contiguous sequence, `[T]`.
///
/// Contiguous here means that elements are laid out so that every element is
/// the same distance from its neighbors.
///
/// Slices are a view into a block of memory represented as a pointer and a
/// length.
template <class T>
class Slice {
 public:
  static constexpr inline Slice from_raw_parts(T* data, usize len) noexcept {
    return Slice(data, len);
  }

  // sus::construct::Into<Slice<T>, T[]> trait.
  template <size_t N>
  static constexpr inline Slice from(T (&data)[N]) {
    return Slice(data, N);
  }

  /// Returns the number of elements in the slice.
  constexpr inline const usize len() const& noexcept { return len_; }

  /// Returns a const reference to the element at index `i`.
  constexpr Option<const T&> get(usize i) const& noexcept {
    if (i < len_) [[likely]]
      return Option<const T&>::some(data_[i.primitive_value]);
    else
      return Option<const T&>::none();
  }
  constexpr Option<const T&> get(usize i) && = delete;

  /// Returns a mutable reference to the element at index `i`.
  constexpr Option<T&> get_mut(usize i) & noexcept
    requires(!std::is_const_v<T>)
  {
    if (i < len_) [[likely]]
      return Option<T&>::some(mref(data_[i.primitive_value]));
    else
      return Option<T&>::none();
  }

  constexpr const T& operator[](usize i) const& noexcept {
    check(i < len_);
    return data_[i.primitive_value];
  }
  constexpr const T& operator[](usize i) && = delete;

  constexpr T& operator[](usize i) & noexcept {
    check(i < len_);
    return data_[i.primitive_value];
  }

  /// Returns a const pointer to the first element in the slice.
  constexpr inline const T* as_ptr() const& noexcept { return data_; }
  const T* as_ptr() && = delete;

  /// Returns a mutable pointer to the first element in the slice.
  constexpr inline T* as_ptr_mut() & noexcept
    requires(!std::is_const_v<T>)
  {
    return data_;
  }

 private:
  constexpr Slice(T* data, usize len) noexcept : data_(data), len_(len) {}

  T* data_;
  usize len_;

  sus_class_never_value_field(unsafe_fn, Slice, data_, nullptr);
};

// Implicit for-ranged loop iteration via `Slice::iter()`.
using ::sus::iter::__private::begin;
using ::sus::iter::__private::end;

}  // namespace sus::containers

namespace sus {
using ::sus::containers::Slice;
}