/*
 * Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
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
#ifndef UNIT_NVGPU_GR_INIT_HAL_GV11B_H
#define UNIT_NVGPU_GR_INIT_HAL_GV11B_H

#include <nvgpu/types.h>

struct gk20a;
struct unit_module;

/** @addtogroup SWUTS-gr-init-hal-gv11b
 *  @{
 *
 * Software Unit Test Specification for common.gr.init HAL
 */

/**
 * Test specification for: test_gr_init_hal_wait_empty.
 *
 * Description: Verify error handling in g->ops.gr.init.wait_empty.
 *
 * Test Type: Feature, Error guessing
 *
 * Targets: g->ops.gr.init.wait_empty.
 *
 * Input: gr_init_setup, gr_init_prepare, gr_init_support must have
 *        been executed successfully.
 *
 * Steps:
 * - Inject timeout error and call g->ops.gr.init.wait_empty.
 *   Should fail since timeout initialization fails.
 * - Set various pass/fail values of gr_status and gr_activity registers
 *   and verify the pass/fail output of g->ops.gr.init.wait_empty as
 *   appropriate.
 *
 * Output: Returns PASS if the steps above were executed successfully. FAIL
 * otherwise.
 */
int test_gr_init_hal_wait_empty(struct unit_module *m,
		struct gk20a *g, void *args);

/**
 * Test specification for: test_gr_init_hal_wait_fe_idle.
 *
 * Description: Verify error handling in g->ops.gr.init.wait_fe_idle.
 *
 * Test Type: Feature, Error guessing
 *
 * Targets: g->ops.gr.init.wait_fe_idle.
 *
 * Input: gr_init_setup, gr_init_prepare, gr_init_support must have
 *        been executed successfully.
 *
 * Steps:
 * - Inject timeout error and call g->ops.gr.init.wait_fe_idle.
 *   Should fail since timeout initialization fails.
 * - Set FE active status in register gr_status_r(), and call
 *   g->ops.gr.init.wait_fe_idle. Should fail since FE fails to idle.
 * - Set FE idle status in register gr_status_r(), and call
 *   g->ops.gr.init.wait_fe_idle. Should pass this time.
 *
 * Output: Returns PASS if the steps above were executed successfully. FAIL
 * otherwise.
 */
int test_gr_init_hal_wait_fe_idle(struct unit_module *m,
		struct gk20a *g, void *args);

/**
 * Test specification for: test_gr_init_hal_fe_pwr_mode.
 *
 * Description: Verify error handling in g->ops.gr.init.fe_pwr_mode_force_on.
 *
 * Test Type: Feature, Error guessing
 *
 * Targets: g->ops.gr.init.fe_pwr_mode_force_on.
 *
 * Input: gr_init_setup, gr_init_prepare, gr_init_support must have
 *        been executed successfully.
 *
 * Steps:
 * - Inject timeout error and call g->ops.gr.init.fe_pwr_mode_force_on.
 *   should fail since timeout initialization fails.
 * - Disable timeout error injection.
 * - Call g->ops.gr.init.fe_pwr_mode_force_on. By default this should
 *   timeout and return error.
 * - Enable readl function error injection and call
 *   g->ops.gr.init.fe_pwr_mode_force_on. Now this should return success.
 * - Also call g->ops.gr.init.fe_pwr_mode_force_on with force flag set to
 *   false. Should return success.
 * - Disable readl function error injection.
 *
 * Output: Returns pass if the steps above were executed successfully. fail
 * otherwise.
 */
int test_gr_init_hal_fe_pwr_mode(struct unit_module *m,
		struct gk20a *g, void *args);

/**
 * Test specification for: test_gr_init_hal_ecc_scrub_reg.
 *
 * Description: Verify error handling in gops.gr.init.ecc_scrub_reg function.
 *
 * Test Type: Feature, Error guessing
 *
 * Targets: g->ops.gr.init.ecc_scrub_reg.
 *
 * Input: gr_init_setup, gr_init_prepare, gr_init_support must have
 *        been executed successfully.
 *
 * Steps:
 * - Disable feature flags for common.gr ECC handling for code coverage
 *   and call g->ops.gr.init.ecc_scrub_reg.
 * - Re-enable all the feature flags.
 * - Inject timeout initialization failures and call
 *   g->ops.gr.init.ecc_scrub_reg.
 * - Set incorrect values of scrub_done for each error type so that scrub
 *   wait times out.
 * - Ensure that g->ops.gr.init.ecc_scrub_reg returns error.
 * - Set correct values of scrub_done for each error so that scrub wait
 *   is successful again.
 *
 * Output: Returns PASS if the steps above were executed successfully. FAIL
 * otherwise.
 */
int test_gr_init_hal_ecc_scrub_reg(struct unit_module *m,
		struct gk20a *g, void *args);

/**
 * Test specification for: test_gr_init_hal_config_error_injection.
 *
 * Description: Verify error handling in gr.init HAL functions that
 *              require tweaks to gr engine configuration.
 *
 * Test Type: Feature, Error guessing
 *
 * Targets: g->ops.gr.init.get_nonpes_aware_tpc,
 *          g->ops.gr.init.sm_id_config,
 *          g->ops.gr.init.fs_state,
 *          g->ops.gr.init.get_attrib_cb_size,
 *          g->ops.gr.init.get_alpha_cb_size,
 *          g->ops.gr.init.pd_skip_table_gpc,
 *          g->ops.gr.init.load_sw_veid_bundle,
 *          g->ops.gr.init.load_sw_bundle_init,
 *          g->ops.gr.init.load_method_init.
 *
 * Input: gr_init_setup, gr_init_prepare, gr_init_support must have
 *        been executed successfully.
 *
 * Steps:
 * - Set gpc_ppc_count to 0 and call g->ops.gr.init.get_nonpes_aware_tpc
 *   for code coverage of the for loop.
 * - Set num_tpc to 2 and num_sm to 4 and call g->ops.gr.init.sm_id_config
 *   to trigger certain error conditions on sm count.
 * - Use combinations of A01 SOC version and GPU chip id and call
 *   g->ops.gr.init.fs_state to cover soc and chip specific code.
 * - Define local function that returns max value and set it to get default
 *   size of alpha_cb and attrib_cb. Then call g->ops.gr.init.get_attrib_cb_size
 *   and g->ops.gr.init.get_alpha_cb_size and verify if expected size is
 *   returned in response.
 * - Set gpc_skip_masks for all the GPCs and call g->ops.gr.init.pd_skip_table_gpc.
 *   Ensure that skip mask is reflected in h/w register.
 *   Unset all the gpc_skip_masks and ensure skip mask is unset in h/w register.
 *   Skip mask should be zero in h/w register only if all the skip masks are zero.
 * - Update g->ops.gr.init.wait_idle to return error, and call
 *   g->ops.gr.init.load_sw_veid_bundle. Make sure it returns error.
 * - Update g->ops.gr.init.wait_idle and g->ops.gr.init.wait_fe_idle to return
 *   error, and call g->ops.gr.init.load_sw_bundle_init. Make sure it returns
 *   error.
 * - Set load_method_init bundle count to zero, and ensure no register write
 *   was performed. Restore the count, and then ensure register update was
 *   performed.
 *
 * Output: Returns PASS if the steps above were executed successfully. FAIL
 * otherwise.
 */
int test_gr_init_hal_config_error_injection(struct unit_module *m,
		struct gk20a *g, void *args);

/**
 * Test specification for: test_gr_init_hal_error_injection.
 *
 * Description: Code coverage test for g->ops.gr.init.commit_global_pagepool.
 *
 * Test Type: Feature, Error guessing
 *
 * Targets: g->ops.gr.init.commit_global_pagepool.
 *
 * Input: gr_init_setup, gr_init_prepare, gr_init_support must have
 *        been executed successfully.
 *
 * Steps:
 * - Setup VM for testing.
 * - Setup gr_ctx and patch_ctx for testing.
 * - Call g->ops.gr.init.commit_global_pagepool with global_ctx flag set
 *   to false and with arbitrary size.
 * - Read back size from register and ensure correct size is set.
 * - Cleanup temporary resources.
 *
 * Output: Returns PASS if the steps above were executed successfully. FAIL
 * otherwise.
 */
int test_gr_init_hal_error_injection(struct unit_module *m,
		struct gk20a *g, void *args);

#endif /* UNIT_NVGPU_GR_INIT_HAL_GV11B_H */

/**
 * @}
 */
