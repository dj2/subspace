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

#include "subspace/mem/forward.h"
#include "subspace/mem/move.h"
#include "subspace/mem/mref.h"

namespace sus::result {
template <class T, class E>
class Result;
}

namespace sus::result::__private {

template <class T>
struct OkMarker {
  sus_clang_bug_54040(constexpr inline OkMarker(T&& value)
                      : value(::sus::forward<T>(value)){});

  T&& value;

  // If the Result's type can construct from a const ref `value` (roughly, is
  // copy-constructible, but may change types), then the marker can do the same.
  //
  // This largely exists to support use in Gtest's EXPECT_EQ, which uses them as
  // a const&, since marker types should normally be converted quickly to the
  // concrete type.
  template <class U, class E>
    requires(std::constructible_from<U, const std::remove_reference_t<T>&>)
  inline constexpr operator ::sus::result::Result<U, E>() const& noexcept {
    return Result<U, E>::with(value);
  }

  template <class U, class E>
  inline constexpr operator ::sus::result::Result<U, E>() && noexcept {
    return Result<U, E>::with(::sus::forward<T>(value));
  }

  // TODO: Make Result hold references and remove the remove_reference_t.
  template <class E>
  inline constexpr ::sus::result::Result<std::remove_reference_t<T>,
                                         std::remove_reference_t<E>>
  construct() && noexcept {
    return ::sus::move(*this);
  }

  // TODO: Make Result hold references and remove the remove_reference_t.
  template <class U, class E>
  inline constexpr ::sus::result::Result<std::remove_reference_t<U>,
                                         std::remove_reference_t<E>>
  construct() && noexcept {
    return ::sus::move(*this);
  }
};

template <class E>
struct ErrMarker {
  sus_clang_bug_54040(constexpr inline ErrMarker(E&& value)
                      : value(::sus::forward<E>(value)){});

  E&& value;

  // If the Result's type can construct from a const ref `value` (roughly, is
  // copy-constructible, but may change types), then the marker can do the same.
  //
  // This largely exists to support use in Gtest's EXPECT_EQ, which uses them as
  // a const&, since marker types should normally be converted quickly to the
  // concrete type.
  template <class T, class F>
    requires(std::constructible_from<F, const std::remove_reference_t<E>&>)
  inline constexpr operator ::sus::result::Result<T, F>() const& noexcept {
    return Result<T, F>::with_err(value);
  }

  template <class T, class F>
  inline constexpr operator ::sus::result::Result<T, F>() && noexcept {
    return Result<T, F>::with_err(::sus::forward<E>(value));
  }

  // TODO: Make Result hold references and remove the remove_reference_t.
  template <class T>
  inline constexpr ::sus::result::Result<std::remove_reference_t<T>,
                                         std::remove_reference_t<E>>
  construct() && noexcept {
    return ::sus::move(*this);
  }

  // TODO: Make Result hold references and remove the remove_reference_t.
  template <class T, class F>
  inline constexpr ::sus::result::Result<std::remove_reference_t<T>,
                                         std::remove_reference_t<F>>
  construct() && noexcept {
    return ::sus::move(*this);
  }
};

}  // namespace sus::result::__private
