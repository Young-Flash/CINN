// Copyright (c) 2022 CINN Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <string>
#include <vector>

#include "cinn/common/target.h"
#include "cinn/hlir/framework/tensor.h"
#include "cinn/runtime/cinn_runtime.h"

namespace cinn {
namespace runtime {

void cinn_assert_true(void* v_args, int msg, bool only_warning, void* stream = nullptr);

void cinn_call_cholesky_host(void* v_args, int num_args, int batch_size, int m, bool upper);

}  // namespace runtime
}  // namespace cinn
