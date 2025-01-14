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

#include "subspace/containers/array.h"
#include "subspace/num/unsigned_integer.h"
#include "subspace/ptr/copy.h"

namespace sus::num {

_sus__unsigned_out_of_line_impl(u8, /*PrimitiveT=*/uint8_t, /*SignedT=*/i8);

_sus__unsigned_out_of_line_impl(u16, /*PrimitiveT=*/uint16_t, /*SignedT=*/i16);

_sus__unsigned_out_of_line_impl(u32, /*PrimitiveT=*/uint32_t, /*SignedT=*/i32);

_sus__unsigned_out_of_line_impl(u64, /*PrimitiveT=*/uint64_t,
                                /*SignedT=*/i64);

_sus__unsigned_out_of_line_impl(
    usize,
    /*PrimitiveT=*/::sus::num::__private::ptr_type<>::unsigned_type,
    /*SignedT=*/isize);

}  // namespace sus::num
