/*
 * general p state infrastructure
 *
 * Copyright (c) 2016-2019, NVIDIA CORPORATION.  All rights reserved.
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

#include <nvgpu/bios.h>
#include <nvgpu/gk20a.h>
#include <nvgpu/pmu.h>
#include <nvgpu/pmu/clk.h>
#include <nvgpu/pmu/pmgr.h>
#include <nvgpu/pmu/therm.h>
#include <nvgpu/pmu/perf.h>

#include "pstate.h"

static int pstate_sw_setup(struct gk20a *g);

void gk20a_deinit_pstate_support(struct gk20a *g)
{
	pmgr_pmu_free_pmupstate(g);
	therm_pmu_free_pmupstate(g);
	perf_pmu_free_pmupstate(g);
	clk_free_pmupstate(g);

	if (g->ops.clk.mclk_deinit != NULL) {
		g->ops.clk.mclk_deinit(g);
	}
}

/*sw setup for pstate components*/
int gk20a_init_pstate_support(struct gk20a *g)
{
	int err;

	nvgpu_log_fn(g, " ");

	err = nvgpu_pmu_wait_ready(g);
	if (err != 0) {
		nvgpu_err(g, "PMU not ready to process pstate requests");
		return err;
	}

	err = clk_init_pmupstate(g);
	if (err != 0) {
		return err;
	}

	err = perf_pmu_init_pmupstate(g);
	if (err != 0) {
		goto err_clk_init_pmupstate;
	}

	err = therm_pmu_init_pmupstate(g);
	if (err != 0) {
		goto err_perf_pmu_init_pmupstate;
	}

	err = pmgr_pmu_init_pmupstate(g);
	if (err != 0) {
		goto err_therm_pmu_init_pmupstate;
	}

	err = volt_rail_sw_setup(g);
	if (err != 0) {
		goto err_pmgr_pmu_init_pmupstate;
	}

	err = volt_dev_sw_setup(g);
	if (err != 0) {
		goto err_pmgr_pmu_init_pmupstate;
	}

	err = volt_policy_sw_setup(g);
	if (err != 0) {
		goto err_pmgr_pmu_init_pmupstate;
	}

	err = clk_vin_sw_setup(g);
	if (err != 0) {
		goto err_pmgr_pmu_init_pmupstate;
	}

	err = clk_fll_sw_setup(g);
	if (err != 0) {
		goto err_pmgr_pmu_init_pmupstate;
	}

	err = therm_domain_sw_setup(g);
	if (err != 0) {
		goto err_pmgr_pmu_init_pmupstate;
	}

	if (g->ops.pmu_perf.support_vfe) {
		err = vfe_var_sw_setup(g);
		if (err != 0) {
			goto err_pmgr_pmu_init_pmupstate;
		}

		err = vfe_equ_sw_setup(g);
		if (err != 0) {
			goto err_pmgr_pmu_init_pmupstate;
		}
	}

	err = clk_domain_sw_setup(g);
	if (err != 0) {
		goto err_pmgr_pmu_init_pmupstate;
	}

	if (g->ops.clk.support_vf_point &&
		g->ops.pmu_perf.support_vfe) {
		err = clk_vf_point_sw_setup(g);
		if (err != 0) {
			goto err_pmgr_pmu_init_pmupstate;
		}
	}

	err = clk_prog_sw_setup(g);
	if (err != 0) {
		goto err_pmgr_pmu_init_pmupstate;
	}

	if (g->ops.clk.support_clk_freq_domain) {
		err = nvgpu_clk_freq_domain_sw_setup(g);
		if (err != 0) {
			goto err_pmgr_pmu_init_pmupstate;
		}
	}

	err = pstate_sw_setup(g);
	if (err != 0) {
		goto err_pmgr_pmu_init_pmupstate;
	}

	if(g->ops.clk.support_pmgr_domain) {
		err = pmgr_domain_sw_setup(g);
		if (err != 0) {
			goto err_pmgr_pmu_init_pmupstate;
		}
	}

	if (g->ops.clk.support_clk_freq_controller) {
		err = clk_freq_controller_sw_setup(g);
		if (err != 0) {
			goto err_pmgr_pmu_init_pmupstate;
		}
	}

	if(g->ops.clk.support_lpwr_pg) {
		err = nvgpu_lpwr_pg_setup(g);
		if (err != 0) {
			goto err_pmgr_pmu_init_pmupstate;
		}
	}

	if(g->ops.pmu_perf.support_changeseq) {
		err = nvgpu_perf_change_seq_sw_setup(g);
		if (err != 0) {
			goto err_clk_init_pmupstate;
		}
	}

	return 0;

err_pmgr_pmu_init_pmupstate:
	pmgr_pmu_free_pmupstate(g);
err_therm_pmu_init_pmupstate:
	therm_pmu_free_pmupstate(g);
err_perf_pmu_init_pmupstate:
	perf_pmu_free_pmupstate(g);
err_clk_init_pmupstate:
	clk_free_pmupstate(g);

	return err;
}

/*sw setup for pstate components*/
int gk20a_init_pstate_pmu_support(struct gk20a *g)
{
	int err;

	nvgpu_log_fn(g, " ");

	if (g->ops.clk.mclk_init != NULL) {
		err = g->ops.clk.mclk_init(g);
		if (err != 0) {
			nvgpu_err(g, "failed to set mclk");
			/* Indicate error and continue */
		}
	}

	err = volt_rail_pmu_setup(g);
	if (err != 0) {
		return err;
	}

	err = volt_dev_pmu_setup(g);
	if (err != 0) {
		return err;
	}

	err = volt_policy_pmu_setup(g);
	if (err != 0) {
		return err;
	}

	err = g->ops.pmu_ver.volt.volt_send_load_cmd_to_pmu(g);
	if (err != 0) {
		nvgpu_err(g,
			"Failed to send VOLT LOAD CMD to PMU: status = 0x%08x.",
			err);
		return err;
	}

	err = therm_domain_pmu_setup(g);
	if (err != 0) {
		return err;
	}

	if (g->ops.pmu_perf.support_vfe) {
		err = vfe_var_pmu_setup(g);
		if (err != 0) {
			return err;
		}

		err = vfe_equ_pmu_setup(g);
		if (err != 0) {
			return err;
		}
	}

	err = clk_domain_pmu_setup(g);
	if (err != 0) {
		return err;
	}

	err = clk_prog_pmu_setup(g);
	if (err != 0) {
		return err;
	}

	err = clk_vin_pmu_setup(g);
	if (err != 0) {
		return err;
	}

	if (g->ops.clk.support_clk_freq_domain) {
		err = nvgpu_clk_freq_domain_pmu_setup(g);
		if (err != 0) {
			return err;
		}
	}

	err = clk_fll_pmu_setup(g);
	if (err != 0) {
		return err;
	}

	if (g->ops.clk.support_clk_freq_controller) {
		err = clk_freq_controller_pmu_setup(g);
		if (err != 0) {
			return err;
		}
	}

	if (g->ops.clk.support_vf_point &&
		g->ops.pmu_perf.support_vfe) {
		err = clk_vf_point_pmu_setup(g);
		if (err != 0) {
			return err;
		}
	}

	err = clk_pmu_vin_load(g);
	if (err != 0) {
		return err;
	}

	if (g->ops.clk.support_clk_freq_domain) {
		err = clk_pmu_clk_domains_load(g);
		if (err != 0) {
			return err;
		}
	}

	if (g->ops.clk.support_pmgr_domain) {
		err = pmgr_domain_pmu_setup(g);
	}

	if(g->ops.pmu_perf.support_changeseq) {
		err = nvgpu_perf_change_seq_pmu_setup(g);
		if (err != 0) {
			return err;
		}
	}

	if (g->ops.pmu_perf.support_vfe) {
		err = g->ops.clk.perf_pmu_vfe_load(g);
		if (err != 0) {
			return err;
		}
	}
	return err;
}

static int pstate_construct_super(struct gk20a *g, struct boardobj **ppboardobj,
				u16 size, void *args)
{
	struct pstate *ptmppstate = (struct pstate *)args;
	struct pstate *pstate;
	int err;

	err = boardobj_construct_super(g, ppboardobj, size, args);
	if (err != 0) {
		return err;
	}

	pstate = (struct pstate *)*ppboardobj;

	pstate->num = ptmppstate->num;
	pstate->clklist = ptmppstate->clklist;
	pstate->lpwr_entry_idx = ptmppstate->lpwr_entry_idx;

	return 0;
}

static int pstate_construct_3x(struct gk20a *g, struct boardobj **ppboardobj,
				u16 size, void *args)
{
	struct boardobj  *ptmpobj = (struct boardobj *)args;

	ptmpobj->type_mask |= BIT32(CTRL_PERF_PSTATE_TYPE_3X);
	return pstate_construct_super(g, ppboardobj, size, args);
}

static struct pstate *pstate_construct(struct gk20a *g, void *args)
{
	struct pstate *pstate = NULL;
	struct pstate *tmp = (struct pstate *)args;

	if ((tmp->super.type != CTRL_PERF_PSTATE_TYPE_3X) ||
	    (pstate_construct_3x(g, (struct boardobj **)&pstate,
			    (u16)sizeof(struct pstate), args) != 0)) {
		nvgpu_err(g,
			"error constructing pstate num=%u", tmp->num);
	}

	return pstate;
}

static int pstate_insert(struct gk20a *g, struct pstate *pstate, u8 index)
{
	struct pstates *pstates = &(g->perf_pmu->pstatesobjs);
	int err;

	err = boardobjgrp_objinsert(&pstates->super.super,
			(struct boardobj *)pstate, index);
	if (err != 0) {
		nvgpu_err(g,
			  "error adding pstate boardobj %d", index);
		return err;
	}

	pstates->num_levels++;

	return err;
}

static int parse_pstate_entry_5x(struct gk20a *g,
		struct vbios_pstate_header_5x *hdr,
		struct vbios_pstate_entry_5x *entry,
		struct pstate *pstate)
{
	u8 *p = (u8 *)entry;
	u32 clkidx;

	p += hdr->base_entry_size;

	(void) memset(pstate, 0, sizeof(struct pstate));
	pstate->super.type = CTRL_PERF_PSTATE_TYPE_3X;
	pstate->num = 0x0FU - U32(entry->pstate_level);
	pstate->clklist.num_info = hdr->clock_entry_count;
	pstate->lpwr_entry_idx = entry->lpwr_entry_idx;

	nvgpu_log_info(g, "pstate P%u", pstate->num);

	for (clkidx = 0; clkidx < hdr->clock_entry_count; clkidx++) {
		struct clk_set_info *pclksetinfo;
		struct vbios_pstate_entry_clock_5x *clk_entry;
		struct clk_domain *clk_domain;

		clk_domain = (struct clk_domain *)BOARDOBJGRP_OBJ_GET_BY_IDX(
			    &g->clk_pmu->clk_domainobjs.super.super, clkidx);

		pclksetinfo = &pstate->clklist.clksetinfo[clkidx];
		clk_entry = (struct vbios_pstate_entry_clock_5x *)p;

		pclksetinfo->clkwhich = clk_domain->domain;
		pclksetinfo->nominal_mhz =
			BIOS_GET_FIELD(u32, clk_entry->param0,
				VBIOS_PSTATE_5X_CLOCK_PROG_PARAM0_NOM_FREQ_MHZ);
		pclksetinfo->min_mhz =
			BIOS_GET_FIELD(u16, clk_entry->param1,
				VBIOS_PSTATE_5X_CLOCK_PROG_PARAM1_MIN_FREQ_MHZ);
		pclksetinfo->max_mhz =
			BIOS_GET_FIELD(u16, clk_entry->param1,
				VBIOS_PSTATE_5X_CLOCK_PROG_PARAM1_MAX_FREQ_MHZ);

		nvgpu_log_info(g,
			"clk_domain=%u nominal_mhz=%u min_mhz=%u max_mhz=%u",
			pclksetinfo->clkwhich, pclksetinfo->nominal_mhz,
			pclksetinfo->min_mhz, pclksetinfo->max_mhz);

		p += hdr->clock_entry_size;
	}

	return 0;
}

static int parse_pstate_table_5x(struct gk20a *g,
		struct vbios_pstate_header_5x *hdr)
{
	struct pstate _pstate, *pstate;
	struct vbios_pstate_entry_5x *entry;
	u32 entry_size;
	u8 i;
	u8 *p = (u8 *)hdr;
	int err = 0;

	if ((hdr->header_size != VBIOS_PSTATE_HEADER_5X_SIZE_10) ||
		(hdr->base_entry_count == 0U) ||
		((hdr->base_entry_size != VBIOS_PSTATE_BASE_ENTRY_5X_SIZE_2) &&
		 (hdr->base_entry_size != VBIOS_PSTATE_BASE_ENTRY_5X_SIZE_3) &&
		 (hdr->base_entry_size != VBIOS_PSTATE_BASE_ENTRY_6X_SIZE_5)) ||
		(hdr->clock_entry_size != VBIOS_PSTATE_CLOCK_ENTRY_5X_SIZE_6) ||
		(hdr->clock_entry_count > CLK_SET_INFO_MAX_SIZE)) {
		return -EINVAL;
	}

	p += hdr->header_size;

	entry_size = U32(hdr->base_entry_size) +
			U32(hdr->clock_entry_count) *
			U32(hdr->clock_entry_size);

	for (i = 0; i < hdr->base_entry_count; i++, p += entry_size) {
		entry = (struct vbios_pstate_entry_5x *)p;

		if (entry->pstate_level == VBIOS_PERFLEVEL_SKIP_ENTRY) {
			continue;
		}

		err = parse_pstate_entry_5x(g, hdr, entry, &_pstate);
		if (err != 0) {
			goto done;
		}

		pstate = pstate_construct(g, &_pstate);
		if (pstate == NULL) {
			goto done;
		}

		err = pstate_insert(g, pstate, i);
		if (err != 0) {
			goto done;
		}
	}

done:
	return err;
}

static int pstate_sw_setup(struct gk20a *g)
{
	struct vbios_pstate_header_5x *hdr = NULL;
	int err = 0;

	nvgpu_log_fn(g, " ");

	nvgpu_cond_init(&g->perf_pmu->pstatesobjs.pstate_notifier_wq);

	err = nvgpu_mutex_init(&g->perf_pmu->pstatesobjs.pstate_mutex);
	if (err != 0) {
		return err;
	}

	err = boardobjgrpconstruct_e32(g, &g->perf_pmu->pstatesobjs.super);
	if (err != 0) {
		nvgpu_err(g,
			  "error creating boardobjgrp for pstates, err=%d",
			  err);
		goto done;
	}

	hdr = (struct vbios_pstate_header_5x *)
			nvgpu_bios_get_perf_table_ptrs(g,
			g->bios.perf_token, PERFORMANCE_TABLE);

	if (hdr == NULL) {
		nvgpu_err(g, "performance table not found");
		err = -EINVAL;
		goto done;
	}

	if (hdr->version != VBIOS_PSTATE_TABLE_VERSION_5X &&
		hdr->version != VBIOS_PSTATE_TABLE_VERSION_6X) {
		nvgpu_err(g, "unknown/unsupported clocks table version=0x%02x",
				hdr->version);
		err = -EINVAL;
		goto done;
	}

	err = parse_pstate_table_5x(g, hdr);
done:
	if (err != 0) {
		nvgpu_mutex_destroy(&g->perf_pmu->pstatesobjs.pstate_mutex);
	}
	return err;
}

struct pstate *pstate_find(struct gk20a *g, u32 num)
{
	struct pstates *pstates = &(g->perf_pmu->pstatesobjs);
	struct pstate *pstate;
	u8 i;

	nvgpu_log_info(g, "pstates = %p", pstates);

	BOARDOBJGRP_FOR_EACH(&pstates->super.super,
			struct pstate *, pstate, i) {
		nvgpu_log_info(g, "pstate=%p num=%u (looking for num=%u)",
				pstate, pstate->num, num);
		if (pstate->num == num) {
			return pstate;
		}
	}
	return NULL;
}

struct clk_set_info *pstate_get_clk_set_info(struct gk20a *g,
		u32 pstate_num, u32 clkwhich)
{
	struct pstate *pstate = pstate_find(g, pstate_num);
	struct clk_set_info *info;
	u32 clkidx;

	nvgpu_log_info(g, "pstate = %p", pstate);

	if (pstate == NULL) {
		return NULL;
	}

	for (clkidx = 0; clkidx < pstate->clklist.num_info; clkidx++) {
		info = &pstate->clklist.clksetinfo[clkidx];
		if (info->clkwhich == clkwhich) {
			return info;
		}
	}
	return NULL;
}
