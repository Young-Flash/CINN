// Copyright (c) 2023 CINN Authors. All Rights Reserved.
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

#include <gflags/gflags.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "absl/types/variant.h"
#include "cinn/common/cas.h"
#include "cinn/common/cinn_value.h"
#include "cinn/common/common.h"
#include "cinn/common/context.h"
#include "cinn/common/ir_util.h"
#include "cinn/common/macros.h"
#include "cinn/common/target.h"
#include "cinn/hlir/framework/node.h"
#include "cinn/hlir/framework/op.h"
#include "cinn/hlir/framework/op_strategy.h"
#include "cinn/hlir/op/op_util.h"
#include "cinn/hlir/pe/elementwise.h"
#include "cinn/hlir/pe/ir_schedule_pe.h"
#include "cinn/hlir/pe/nn.h"
#include "cinn/hlir/pe/schedule.h"
#include "cinn/ir/ir.h"
#include "cinn/ir/ir_base.h"
#include "cinn/ir/ir_operators.h"
#include "cinn/ir/tensor.h"
#include "cinn/lang/builtin.h"
#include "cinn/lang/compute.h"
#include "cinn/lang/packed_func.h"
#include "cinn/poly/stage.h"
#include "glog/logging.h"

namespace cinn {
namespace hlir {
namespace op {

using common::CINNValue;
using common::CINNValuePack;

std::shared_ptr<framework::OpStrategy> StrategyForRandInt(const framework::NodeAttr &attrs,
                                                          const std::vector<ir::Tensor> &inputs,
                                                          const std::vector<Type> &out_type,
                                                          const std::vector<std::vector<int>> &output_shapes,
                                                          const Target &target) {
  framework::CINNCompute randint_compute([=](lang::Args args, lang::RetValue *ret) {
    CHECK(attrs.attr_store.count("shape"));
    ir::Tensor shape_tensor;
    std::string tensor_name = "randint_out";
    auto out                = pe::Identity(shape_tensor, tensor_name).front();
    auto stages             = CreateStages({out});
    std::vector<CINNValue> res{CINNValue(out), CINNValue(stages)};
    *ret = CINNValuePack{res};
  });
  auto strategy = std::make_shared<framework::OpStrategy>();
  strategy->AddImpl(randint_compute, GetInjectiveScheduleFunc(output_shapes, target), "strategy.randint.x86", 1);
  return strategy;
}

std::vector<framework::shape_t> InferShapeForRandInt(const std::vector<framework::shape_t> &inputs_shape,
                                                     const framework::AttrMapType &attrs) {
  CHECK(attrs.count("shape"));
  auto shape = absl::get<std::vector<int>>(attrs.at("shape"));
  CHECK(!shape.empty()) << "shape attr is empty!";
  return {shape};
}

std::vector<Type> InferDtypeForRandInt(const std::vector<Type> &inputs_type, const framework::AttrMapType &attrs) {
  std::string dtype = "int64";
  if (attrs.find("dtype") != attrs.end()) {
    dtype = absl::get<std::string>(attrs.at("dtype"));
  }
  CHECK(dtype == "int32" || dtype == "int64") << "randint dtype must be int32 or int64 but received dtype = " << dtype;
  std::vector<Type> res{common::Str2Type(dtype)};
  return res;
}

}  // namespace op
}  // namespace hlir
}  // namespace cinn

CINN_REGISTER_HELPER(randint_ops) {
  CINN_REGISTER_OP(randint)
      .describe("RandInt")
      .set_num_inputs(0)
      .set_num_outputs(1)
      .set_attr<cinn::hlir::framework::StrategyFunction>("CINNStrategy", cinn::hlir::op::StrategyForRandInt)
      .set_attr("infershape", MakeOpFunction(cinn::hlir::op::InferShapeForRandInt))
      .set_attr("inferdtype", MakeOpFunction(cinn::hlir::op::InferDtypeForRandInt))
      .set_attr<cinn::hlir::framework::OpPatternKind>("OpPattern", cinn::hlir::framework::OpPatternKind::kNonFusible)
      .set_support_level(4);

  return true;
}
