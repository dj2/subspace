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

#include <type_traits>

#include "assertions/check.h"
#include "marker/unsafe.h"
#include "mem/replace.h"
#include "mem/take.h"
#include "traits/make_default.h"

namespace sus::option {

/// The representation of an Option's state, which can either be #None to
/// represent it has no value, or #Some for when it is holding a value.
enum class State : bool {
  /// The Option is not holding any value.
  None,
  /// The Option is holding a value.
  Some,
};
using State::None;
using State::Some;

/// A type which either holds #Some value of type T, or #None.
template <class T>
class Option {
 public:
  /// Construct an Option that is holding the given value.
  static inline constexpr Option some(T t) noexcept {
    return Option(static_cast<T&&>(t));
  }
  /// Construct an Option that is holding no value.
  static inline constexpr Option none() noexcept { return Option(); }

  /// Construct an Option with the default value for the type it contains.
  ///
  /// The type `T` must be #MakeDefault, and will be constructed through that
  /// trait.
  template <class U = T>
    requires(sus::traits::MakeDefault<U>::has_trait)
  static inline constexpr Option<U> with_default() noexcept {
    return Option<U>::some(sus::traits::MakeDefault<U>::make_default());
  }

  /// Destructor for the Option.
  ///
  /// Destroys the value contained within the option, if there is one.
  constexpr inline ~Option() noexcept {
    if (state_ == State::Some) t_.~T();
  }

  /// Drop the current value from the Option, if there is one.
  ///
  /// Afterward the option will unconditionally be #None.
  constexpr void clear() & noexcept {
    if (::sus::mem::replace(state_, State::None) == State::Some) t_.~T();
  }

  /// Returns whether the Option currently contains a value.
  ///
  /// If there is a value present, it can be extracted with <unwrap>() or
  /// <expect>().
  constexpr bool is_some() const noexcept { return state_ == State::Some; }
  /// Returns whether the Option is currently empty, containing no value.
  constexpr bool is_none() const noexcept { return state_ == State::None; }

  /// An operator which returns the state of the Option, either #Some or #None.
  ///
  /// This supports the use of an Option in a `switch()`, allowing it to act as
  /// a tagged union between "some value" and "no value".
  ///
  /// # Example
  ///
  /// ```cpp
  /// auto x = Option<int>::some();
  /// switch (x) {
  ///  case Some:
  ///   return static_cast<decltype(x)&&>(x).unwrap_unchecked(unsafe_fn);
  ///  case None:
  ///   return -1;
  /// }
  /// ```
  operator State() { return state_; }

  /// Unwraps the contained value inside the Option.
  ///
  /// The function will panic with the given message if the Option's state is
  /// currently `None`.
  constexpr T expect(/* TODO: string view type */ const char* msg) && noexcept
      [[nonnull]] {
    ::sus::check_with_message(state_ == State::Some, *msg);
    return static_cast<Option&&>(*this).unwrap_unchecked(unsafe_fn);
  }
  /// Unwraps the contained value inside the Option.
  ///
  /// The function will panic without a message if the Option's state is
  /// currently `None`.
  constexpr T unwrap() && noexcept {
    ::sus::check(state_ == State::Some);
    return static_cast<Option&&>(*this).unwrap_unchecked(unsafe_fn);
  }
  /// Unwraps the contained value inside the Option.
  ///
  /// # Safety
  ///
  /// It is Undefined Behaviour to call this function when the Option's state is
  /// `None`. The caller is responsible for ensuring the Option contains a value
  /// beforehand, and the safer <unwrap>() or <expect>() should almost always be
  /// preferred.
  constexpr inline T unwrap_unchecked(
      ::sus::marker::UnsafeFnMarker) && noexcept {
    state_ = State::None;
    return static_cast<T&&>(t_);
  }

  /// Returns a new Option containing whatever was inside the current Option.
  ///
  /// If this Option contains #None then it is left unchanged and returns an
  /// Option containing #None. If this Option contains #Some with a value, the
  /// value is moved into the returned Option and this Option will contain
  /// #None afterward.
  constexpr Option take() & noexcept {
    return (::sus::mem::replace(state_, State::None) == State::Some)
               ? Option::some(sus::mem::take_and_destruct(unsafe_fn, t_))
               : Option::none();
  }

 private:
  /// Constructor for #None.
  constexpr explicit Option() : state_(State::None) {}
  /// Constructor for #Some.
  constexpr explicit Option(T&& t)
      : t_(static_cast<T&&>(t)), state_(State::Some) {}

  // TODO: determine if we can put the tag into `T` from its type (e.g. sizeof
  // < alignment?), and then do so?
  union {
    T t_;
  };

  State state_;
};

}  // namespace sus::option

// Promote Option and its enum values into the `sus` namespace.
namespace sus {
using ::sus::option::None;
using ::sus::option::Option;
using ::sus::option::Some;
}  // namespace sus