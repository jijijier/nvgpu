/*
 * Copyright (c) 2018-2020, NVIDIA CORPORATION.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <nvgpu/io.h>
#include <nvgpu/types.h>
#include <nvgpu/bug.h>
#include <nvgpu/static_analysis.h>
#include <nvgpu/gk20a.h>

void nvgpu_writel_check(struct gk20a *g, u32 r, u32 v)
{
	u32 read_val = 0U;

	nvgpu_writel(g, r, v);
	read_val = nvgpu_readl(g, r);
	if (v != read_val) {
		nvgpu_err(g, "r=0x%x rd=0x%x wr=0x%x (mismatch)",
					r, read_val, v);
		nvgpu_assert(v == read_val);
	}
}

void nvgpu_writel_loop(struct gk20a *g, u32 r, u32 v, u32 retries)
{
	u32 reg_val = 0U;

	nvgpu_writel(g, r, v);

	do {
		retries = nvgpu_safe_sub_u32(retries, 1U);
		reg_val = nvgpu_readl(g, r);
	} while ((reg_val != v) && (retries > 0U));

	nvgpu_assert(reg_val == v);
}
