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

#include "subdoc/lib/database.h"
#include "subdoc/lib/gen/html_writer.h"
#include "subdoc/lib/gen/options.h"

namespace subdoc::gen {

void generate_namespace(const NamespaceElement& element,
                        const Options& options) noexcept;

void generate_namespace_reference(HtmlWriter::OpenDiv& section_div,
                                  const NamespaceElement& element) noexcept;

}  // namespace subdoc::gen
