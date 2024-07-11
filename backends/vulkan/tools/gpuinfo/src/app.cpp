/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <executorch/backends/vulkan/runtime/api/api.h>
#include <executorch/backends/vulkan/runtime/graph/ops/utils/StagingUtils.h>
#include <iostream>

#include "utils.h"

void reg_count() {
  const uint32_t NREG_MIN = 1;

  uint32_t NITER;

  auto bench = [&](uint32_t nthread, uint32_t ngrp, uint32_t nreg) {
    size_t len = sizeof(float);
    StorageBuffer buffer(context(), vkapi::kFloat, len);
    ParamsBuffer params(context(), int32_t(len));
    vkapi::PipelineBarrier pipeline_barrier{};

    auto shader_name = "reg_count_" + std::to_string(nreg);

    auto time = benchmark_on_gpu(shader_name, 100, [&]() {
      context()->submit_compute_job(
          VK_KERNEL_FROM_STR(shader_name),
          pipeline_barrier,
          {nthread, ngrp, 1},
          {nthread, 1, 1},
          {SV(NITER)},
          VK_NULL_HANDLE,
          0,
          buffer.buffer(),
          params.buffer());
    });
    return time;
  };

  std::cout << "Calculating NITER..." << std::endl;
  ensure_min_niter(1000, NITER, [&]() { return bench(1, 1, NREG_MIN); });
  std::cout << "NITER," << NITER << std::endl;
}

int main(int argc, const char** argv) {
  context()->initialize_querypool();

  reg_count();

  return 0;
}
