/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#version 450 core

#define PRECISION ${PRECISION}

layout(std430) buffer;

$if MEMTYPE == "ubo":
    ${layout_declare_ubo(0, "vec4", "A")}
$else:
    ${layout_declare_buffer(0, "r", "A", DTYPE, "PRECISION", False)}

${layout_declare_buffer(1, "w", "B", DTYPE, "PRECISION", False)}

layout(local_size_x_id = 0, local_size_y_id = 1, local_size_z_id = 2) in;

layout(constant_id = 3) const int niter = 1;
layout(constant_id = 4) const int addr_mask = 1;
layout(constant_id = 5) const int local_group_size = 1;

void main() {
    vec4 sum = vec4(0);
    const uint workgroup_width = local_group_size * niter * ${NUNROLL};
    uint offset = (gl_WorkGroupID[0] * workgroup_width  + gl_LocalInvocationID[0]) & addr_mask;

    int i = 0;
    for (; i < niter; ++i)
    {
      $for j in range(int(NUNROLL)):
          sum *= A[offset];
          offset = (offset + local_group_size) & addr_mask;
    }

    vec4 zero = vec4(i>>31);

    B[gl_LocalInvocationID[0]] = sum + zero;
}