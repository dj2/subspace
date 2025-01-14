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

#include "cir/tests/cir_test.h"

TEST_F(CirTest, Smoke) {
  auto output = run_code(R"(
    void f() {
      int i = 2;
    })");
  bool eq = cir_eq(sus::move(output), R"(
    fn f@0() {
      (TODO: body)
    })");
  EXPECT_TRUE(eq);
}
