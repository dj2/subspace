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

#include "subdoc/tests/subdoc_gen_test.h"

TEST_F(SubDocGenTest, FunctionOverloads) {
  EXPECT_TRUE(run_gen_test("function-overloads"));
}

TEST_F(SubDocGenTest, Markdown) {
  EXPECT_TRUE(run_gen_test("markdown"));
}

TEST_F(SubDocGenTest, NestedNamespace) {
  EXPECT_TRUE(run_gen_test("nested-namespace"));
}

TEST_F(SubDocGenTest, StructBasic) {
  EXPECT_TRUE(run_gen_test("struct-basic"));
}

TEST_F(SubDocGenTest, StructComplex) {
  EXPECT_TRUE(run_gen_test("struct-complex"));
}

TEST_F(SubDocGenTest, TypenamesAcrossPaths) {
  EXPECT_TRUE(run_gen_test("typenames-across-paths"));
}
