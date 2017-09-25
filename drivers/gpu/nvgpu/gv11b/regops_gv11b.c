/*
 * Tegra GV11b GPU Driver Register Ops
 *
 * Copyright (c) 2017, NVIDIA CORPORATION.  All rights reserved.
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

#include <linux/slab.h>
#include <linux/err.h>
#include <linux/bsearch.h>
#include <uapi/linux/nvgpu.h>

#include "gk20a/gk20a.h"
#include "gk20a/dbg_gpu_gk20a.h"
#include "gk20a/regops_gk20a.h"
#include "regops_gv11b.h"

static const struct regop_offset_range gv11b_global_whitelist_ranges[] = {
	{ 0x000004f0,   1},
	{ 0x00001a00,   1},
	{ 0x00009400,   1},
	{ 0x00009410,   1},
	{ 0x00009480,   1},
	{ 0x00020200,  32},
	{ 0x00021c04,   2},
	{ 0x00021c14,   3},
	{ 0x00021c24,  71},
	{ 0x00021d44,   1},
	{ 0x00021d4c,   1},
	{ 0x00021d54,   1},
	{ 0x00021d5c,   1},
	{ 0x00021d68,  19},
	{ 0x00021dbc,  16},
	{ 0x00022430,   7},
	{ 0x00022450,   1},
	{ 0x0002245c,   2},
	{ 0x00070000,   5},
	{ 0x000840a8,   1},
	{ 0x00084b5c,   1},
	{ 0x000870a8,   1},
	{ 0x000884e0,   1},
	{ 0x00100c18,   3},
	{ 0x00100c84,   1},
	{ 0x0010a0a8,   1},
	{ 0x0010a4f0,   1},
	{ 0x0013c808,   2},
	{ 0x0013cc14,   1},
	{ 0x0013ec18,   1},
	{ 0x00140028,   1},
	{ 0x00140280,   1},
	{ 0x001402a0,   1},
	{ 0x00140350,   1},
	{ 0x00140480,   1},
	{ 0x001404a0,   1},
	{ 0x00140550,   1},
	{ 0x00140680,   1},
	{ 0x001406a0,   1},
	{ 0x00140750,   1},
	{ 0x00142028,   1},
	{ 0x00142280,   1},
	{ 0x001422a0,   1},
	{ 0x00142350,   1},
	{ 0x00142480,   1},
	{ 0x001424a0,   1},
	{ 0x00142550,   1},
	{ 0x00142680,   1},
	{ 0x001426a0,   1},
	{ 0x00142750,   1},
	{ 0x0017e028,   1},
	{ 0x0017e280,   1},
	{ 0x0017e294,   1},
	{ 0x0017e29c,   2},
	{ 0x0017e2ac,   1},
	{ 0x0017e350,   1},
	{ 0x0017e39c,   1},
	{ 0x0017e480,   1},
	{ 0x0017e4a0,   1},
	{ 0x0017e550,   1},
	{ 0x0017e680,   1},
	{ 0x0017e6a0,   1},
	{ 0x0017e750,   1},
	{ 0x00180040,  41},
	{ 0x001800ec,   1},
	{ 0x001800f8,   7},
	{ 0x00180120,   2},
	{ 0x00180240,  41},
	{ 0x001802ec,   1},
	{ 0x001802f8,   7},
	{ 0x00180320,   2},
	{ 0x00180440,  41},
	{ 0x001804ec,   1},
	{ 0x001804f8,   7},
	{ 0x00180520,   2},
	{ 0x00180640,  41},
	{ 0x001806ec,   1},
	{ 0x001806f8,   7},
	{ 0x00180720,   2},
	{ 0x00180840,  41},
	{ 0x001808ec,   1},
	{ 0x001808f8,   7},
	{ 0x00180920,   2},
	{ 0x00180a40,  41},
	{ 0x00180aec,   1},
	{ 0x00180af8,   7},
	{ 0x00180b20,   2},
	{ 0x00180c40,  41},
	{ 0x00180cec,   1},
	{ 0x00180cf8,   7},
	{ 0x00180d20,   2},
	{ 0x00180e40,  41},
	{ 0x00180eec,   1},
	{ 0x00180ef8,   7},
	{ 0x00180f20,   2},
	{ 0x00181040,  41},
	{ 0x001810ec,   1},
	{ 0x001810f8,   7},
	{ 0x00181120,   2},
	{ 0x00181240,  41},
	{ 0x001812ec,   1},
	{ 0x001812f8,   7},
	{ 0x00181320,   2},
	{ 0x00181440,  41},
	{ 0x001814ec,   1},
	{ 0x001814f8,   7},
	{ 0x00181520,   2},
	{ 0x00181640,  41},
	{ 0x001816ec,   1},
	{ 0x001816f8,   7},
	{ 0x00181720,   2},
	{ 0x00181840,  41},
	{ 0x001818ec,   1},
	{ 0x001818f8,   7},
	{ 0x00181920,   2},
	{ 0x00181a40,  41},
	{ 0x00181aec,   1},
	{ 0x00181af8,   7},
	{ 0x00181b20,   2},
	{ 0x00181c40,  41},
	{ 0x00181cec,   1},
	{ 0x00181cf8,   7},
	{ 0x00181d20,   2},
	{ 0x00181e40,  41},
	{ 0x00181eec,   1},
	{ 0x00181ef8,   7},
	{ 0x00181f20,   2},
	{ 0x00182040,  41},
	{ 0x001820ec,   1},
	{ 0x001820f8,   7},
	{ 0x00182120,   2},
	{ 0x00182240,  41},
	{ 0x001822ec,   1},
	{ 0x001822f8,   7},
	{ 0x00182320,   2},
	{ 0x00182440,  41},
	{ 0x001824ec,   1},
	{ 0x001824f8,   7},
	{ 0x00182520,   2},
	{ 0x00182640,  41},
	{ 0x001826ec,   1},
	{ 0x001826f8,   7},
	{ 0x00182720,   2},
	{ 0x00182840,  41},
	{ 0x001828ec,   1},
	{ 0x001828f8,   7},
	{ 0x00182920,   2},
	{ 0x00182a40,  41},
	{ 0x00182aec,   1},
	{ 0x00182af8,   7},
	{ 0x00182b20,   2},
	{ 0x00182c40,  41},
	{ 0x00182cec,   1},
	{ 0x00182cf8,   7},
	{ 0x00182d20,   2},
	{ 0x00182e40,  41},
	{ 0x00182eec,   1},
	{ 0x00182ef8,   7},
	{ 0x00182f20,   2},
	{ 0x00183040,  41},
	{ 0x001830ec,   1},
	{ 0x001830f8,   7},
	{ 0x00183120,   2},
	{ 0x00183240,  41},
	{ 0x001832ec,   1},
	{ 0x001832f8,   7},
	{ 0x00183320,   2},
	{ 0x00183440,  41},
	{ 0x001834ec,   1},
	{ 0x001834f8,   7},
	{ 0x00183520,   2},
	{ 0x00183640,  41},
	{ 0x001836ec,   1},
	{ 0x001836f8,   7},
	{ 0x00183720,   2},
	{ 0x00183840,  41},
	{ 0x001838ec,   1},
	{ 0x001838f8,   7},
	{ 0x00183920,   2},
	{ 0x00183a40,  41},
	{ 0x00183aec,   1},
	{ 0x00183af8,   7},
	{ 0x00183b20,   2},
	{ 0x00183c40,  41},
	{ 0x00183cec,   1},
	{ 0x00183cf8,   7},
	{ 0x00183d20,   2},
	{ 0x00183e40,  41},
	{ 0x00183eec,   1},
	{ 0x00183ef8,   7},
	{ 0x00183f20,   2},
	{ 0x001c80a8,   1},
	{ 0x001c9100,   1},
	{ 0x001cc0a8,   1},
	{ 0x001cd100,   1},
	{ 0x001d00a8,   1},
	{ 0x001d1100,   1},
	{ 0x00200040,  41},
	{ 0x002000ec,   1},
	{ 0x002000f8,   7},
	{ 0x00200120,   2},
	{ 0x00200240,  41},
	{ 0x002002ec,   1},
	{ 0x002002f8,   7},
	{ 0x00200320,   2},
	{ 0x00200440,  41},
	{ 0x002004ec,   1},
	{ 0x002004f8,   7},
	{ 0x00200520,   2},
	{ 0x00200640,  41},
	{ 0x002006ec,   1},
	{ 0x002006f8,   7},
	{ 0x00200720,   2},
	{ 0x00200840,  41},
	{ 0x002008ec,   1},
	{ 0x002008f8,   7},
	{ 0x00200920,   2},
	{ 0x00200a40,  41},
	{ 0x00200aec,   1},
	{ 0x00200af8,   7},
	{ 0x00200b20,   2},
	{ 0x00200c40,  41},
	{ 0x00200cec,   1},
	{ 0x00200cf8,   7},
	{ 0x00200d20,   2},
	{ 0x00200e40,  41},
	{ 0x00200eec,   1},
	{ 0x00200ef8,   7},
	{ 0x00200f20,   2},
	{ 0x00201040,  41},
	{ 0x002010ec,   1},
	{ 0x002010f8,   7},
	{ 0x00201120,   2},
	{ 0x00201240,  41},
	{ 0x002012ec,   1},
	{ 0x002012f8,   7},
	{ 0x00201320,   2},
	{ 0x00201440,  41},
	{ 0x002014ec,   1},
	{ 0x002014f8,   7},
	{ 0x00201520,   2},
	{ 0x00201640,  41},
	{ 0x002016ec,   1},
	{ 0x002016f8,   7},
	{ 0x00201720,   2},
	{ 0x00201840,  41},
	{ 0x002018ec,   1},
	{ 0x002018f8,   7},
	{ 0x00201920,   2},
	{ 0x00201a40,  41},
	{ 0x00201aec,   1},
	{ 0x00201af8,   7},
	{ 0x00201b20,   2},
	{ 0x00201c40,  41},
	{ 0x00201cec,   1},
	{ 0x00201cf8,   7},
	{ 0x00201d20,   2},
	{ 0x00201e40,  41},
	{ 0x00201eec,   1},
	{ 0x00201ef8,   7},
	{ 0x00201f20,   2},
	{ 0x00202040,  41},
	{ 0x002020ec,   1},
	{ 0x002020f8,   7},
	{ 0x00202120,   2},
	{ 0x00202240,  41},
	{ 0x002022ec,   1},
	{ 0x002022f8,   7},
	{ 0x00202320,   2},
	{ 0x00202440,  41},
	{ 0x002024ec,   1},
	{ 0x002024f8,   7},
	{ 0x00202520,   2},
	{ 0x00202640,  41},
	{ 0x002026ec,   1},
	{ 0x002026f8,   7},
	{ 0x00202720,   2},
	{ 0x00202840,  41},
	{ 0x002028ec,   1},
	{ 0x002028f8,   7},
	{ 0x00202920,   2},
	{ 0x00202a40,  41},
	{ 0x00202aec,   1},
	{ 0x00202af8,   7},
	{ 0x00202b20,   2},
	{ 0x00202c40,  41},
	{ 0x00202cec,   1},
	{ 0x00202cf8,   7},
	{ 0x00202d20,   2},
	{ 0x00202e40,  41},
	{ 0x00202eec,   1},
	{ 0x00202ef8,   7},
	{ 0x00202f20,   2},
	{ 0x00203040,  41},
	{ 0x002030ec,   1},
	{ 0x002030f8,   7},
	{ 0x00203120,   2},
	{ 0x00203240,  41},
	{ 0x002032ec,   1},
	{ 0x002032f8,   7},
	{ 0x00203320,   2},
	{ 0x00203440,  41},
	{ 0x002034ec,   1},
	{ 0x002034f8,   7},
	{ 0x00203520,   2},
	{ 0x00203640,  41},
	{ 0x002036ec,   1},
	{ 0x002036f8,   7},
	{ 0x00203720,   2},
	{ 0x00203840,  41},
	{ 0x002038ec,   1},
	{ 0x002038f8,   7},
	{ 0x00203920,   2},
	{ 0x00203a40,  41},
	{ 0x00203aec,   1},
	{ 0x00203af8,   7},
	{ 0x00203b20,   2},
	{ 0x00203c40,  41},
	{ 0x00203cec,   1},
	{ 0x00203cf8,   7},
	{ 0x00203d20,   2},
	{ 0x00203e40,  41},
	{ 0x00203eec,   1},
	{ 0x00203ef8,   7},
	{ 0x00203f20,   2},
	{ 0x00240040,  41},
	{ 0x002400ec,   1},
	{ 0x002400f8,   7},
	{ 0x00240120,   2},
	{ 0x00240240,  41},
	{ 0x002402ec,   1},
	{ 0x002402f8,   7},
	{ 0x00240320,   2},
	{ 0x00240440,  41},
	{ 0x002404ec,   1},
	{ 0x002404f8,   7},
	{ 0x00240520,   2},
	{ 0x00240640,  41},
	{ 0x002406ec,   1},
	{ 0x002406f8,   7},
	{ 0x00240720,   2},
	{ 0x00240840,  41},
	{ 0x002408ec,   1},
	{ 0x002408f8,   7},
	{ 0x00240920,   2},
	{ 0x00240a40,  41},
	{ 0x00240aec,   1},
	{ 0x00240af8,   7},
	{ 0x00240b20,   2},
	{ 0x00240c40,  41},
	{ 0x00240cec,   1},
	{ 0x00240cf8,   7},
	{ 0x00240d20,   2},
	{ 0x00240e40,  41},
	{ 0x00240eec,   1},
	{ 0x00240ef8,   7},
	{ 0x00240f20,   2},
	{ 0x00241040,  41},
	{ 0x002410ec,   1},
	{ 0x002410f8,   7},
	{ 0x00241120,   2},
	{ 0x00241240,  41},
	{ 0x002412ec,   1},
	{ 0x002412f8,   7},
	{ 0x00241320,   2},
	{ 0x00241440,  41},
	{ 0x002414ec,   1},
	{ 0x002414f8,   7},
	{ 0x00241520,   2},
	{ 0x00241640,  41},
	{ 0x002416ec,   1},
	{ 0x002416f8,   7},
	{ 0x00241720,   2},
	{ 0x00241840,  41},
	{ 0x002418ec,   1},
	{ 0x002418f8,   7},
	{ 0x00241920,   2},
	{ 0x00241a40,  41},
	{ 0x00241aec,   1},
	{ 0x00241af8,   7},
	{ 0x00241b20,   2},
	{ 0x00241c40,  41},
	{ 0x00241cec,   1},
	{ 0x00241cf8,   7},
	{ 0x00241d20,   2},
	{ 0x00241e40,  41},
	{ 0x00241eec,   1},
	{ 0x00241ef8,   7},
	{ 0x00241f20,   2},
	{ 0x00242040,  41},
	{ 0x002420ec,   1},
	{ 0x002420f8,   7},
	{ 0x00242120,   2},
	{ 0x00242240,  41},
	{ 0x002422ec,   1},
	{ 0x002422f8,   7},
	{ 0x00242320,   2},
	{ 0x00242440,  41},
	{ 0x002424ec,   1},
	{ 0x002424f8,   7},
	{ 0x00242520,   2},
	{ 0x00242640,  41},
	{ 0x002426ec,   1},
	{ 0x002426f8,   7},
	{ 0x00242720,   2},
	{ 0x00242840,  41},
	{ 0x002428ec,   1},
	{ 0x002428f8,   7},
	{ 0x00242920,   2},
	{ 0x00242a40,  41},
	{ 0x00242aec,   1},
	{ 0x00242af8,   7},
	{ 0x00242b20,   2},
	{ 0x00242c40,  41},
	{ 0x00242cec,   1},
	{ 0x00242cf8,   7},
	{ 0x00242d20,   2},
	{ 0x00242e40,  41},
	{ 0x00242eec,   1},
	{ 0x00242ef8,   7},
	{ 0x00242f20,   2},
	{ 0x00243040,  41},
	{ 0x002430ec,   1},
	{ 0x002430f8,   7},
	{ 0x00243120,   2},
	{ 0x00243240,  41},
	{ 0x002432ec,   1},
	{ 0x002432f8,   7},
	{ 0x00243320,   2},
	{ 0x00243440,  41},
	{ 0x002434ec,   1},
	{ 0x002434f8,   7},
	{ 0x00243520,   2},
	{ 0x00243640,  41},
	{ 0x002436ec,   1},
	{ 0x002436f8,   7},
	{ 0x00243720,   2},
	{ 0x00243840,  41},
	{ 0x002438ec,   1},
	{ 0x002438f8,   7},
	{ 0x00243920,   2},
	{ 0x00243a40,  41},
	{ 0x00243aec,   1},
	{ 0x00243af8,   7},
	{ 0x00243b20,   2},
	{ 0x00243c40,  41},
	{ 0x00243cec,   1},
	{ 0x00243cf8,   7},
	{ 0x00243d20,   2},
	{ 0x00243e40,  41},
	{ 0x00243eec,   1},
	{ 0x00243ef8,   7},
	{ 0x00243f20,   2},
	{ 0x00244000,   1},
	{ 0x00244008,   1},
	{ 0x00244010,   2},
	{ 0x00246000,   1},
	{ 0x00246008,   1},
	{ 0x00246010,   2},
	{ 0x00248000,   1},
	{ 0x00248008,   1},
	{ 0x00248010,   2},
	{ 0x0024a000,   1},
	{ 0x0024a008,   1},
	{ 0x0024a010,  11},
	{ 0x0024a040,   3},
	{ 0x0024a050,   3},
	{ 0x0024a060,   4},
	{ 0x0024a074,   7},
	{ 0x0024a094,   3},
	{ 0x0024a0a4,   1},
	{ 0x0024a100,   6},
	{ 0x00250040,  25},
	{ 0x002500c8,   7},
	{ 0x002500ec,   1},
	{ 0x002500f8,   7},
	{ 0x00250120,   2},
	{ 0x00250240,  25},
	{ 0x002502c8,   7},
	{ 0x002502ec,   1},
	{ 0x002502f8,   7},
	{ 0x00250320,   2},
	{ 0x00250840,  25},
	{ 0x002508c8,   7},
	{ 0x002508ec,   1},
	{ 0x002508f8,   7},
	{ 0x00250920,   2},
	{ 0x00250a40,  25},
	{ 0x00250ac8,   7},
	{ 0x00250aec,   1},
	{ 0x00250af8,   7},
	{ 0x00250b20,   2},
	{ 0x00251800,   3},
	{ 0x00251810,   2},
	{ 0x00251a00,   3},
	{ 0x00251a10,   2},
	{ 0x00278040,  25},
	{ 0x002780c8,   7},
	{ 0x002780ec,   1},
	{ 0x002780f8,   7},
	{ 0x00278120,   2},
	{ 0x00278240,  25},
	{ 0x002782c8,   7},
	{ 0x002782ec,   1},
	{ 0x002782f8,   7},
	{ 0x00278320,   2},
	{ 0x00278440,  25},
	{ 0x002784c8,   7},
	{ 0x002784ec,   1},
	{ 0x002784f8,   7},
	{ 0x00278520,   2},
	{ 0x00278640,  25},
	{ 0x002786c8,   7},
	{ 0x002786ec,   1},
	{ 0x002786f8,   7},
	{ 0x00278720,   2},
	{ 0x00278840,  25},
	{ 0x002788c8,   7},
	{ 0x002788ec,   1},
	{ 0x002788f8,   7},
	{ 0x00278920,   2},
	{ 0x00278a40,  25},
	{ 0x00278ac8,   7},
	{ 0x00278aec,   1},
	{ 0x00278af8,   7},
	{ 0x00278b20,   2},
	{ 0x00278c40,  25},
	{ 0x00278cc8,   7},
	{ 0x00278cec,   1},
	{ 0x00278cf8,   7},
	{ 0x00278d20,   2},
	{ 0x00278e40,  25},
	{ 0x00278ec8,   7},
	{ 0x00278eec,   1},
	{ 0x00278ef8,   7},
	{ 0x00278f20,   2},
	{ 0x00279040,  25},
	{ 0x002790c8,   7},
	{ 0x002790ec,   1},
	{ 0x002790f8,   7},
	{ 0x00279120,   2},
	{ 0x00279240,  25},
	{ 0x002792c8,   7},
	{ 0x002792ec,   1},
	{ 0x002792f8,   7},
	{ 0x00279320,   2},
	{ 0x00279440,  25},
	{ 0x002794c8,   7},
	{ 0x002794ec,   1},
	{ 0x002794f8,   7},
	{ 0x00279520,   2},
	{ 0x00279640,  25},
	{ 0x002796c8,   7},
	{ 0x002796ec,   1},
	{ 0x002796f8,   7},
	{ 0x00279720,   2},
	{ 0x00279840,  25},
	{ 0x002798c8,   7},
	{ 0x002798ec,   1},
	{ 0x002798f8,   7},
	{ 0x00279920,   2},
	{ 0x00279a40,  25},
	{ 0x00279ac8,   7},
	{ 0x00279aec,   1},
	{ 0x00279af8,   7},
	{ 0x00279b20,   2},
	{ 0x00279c40,  25},
	{ 0x00279cc8,   7},
	{ 0x00279cec,   1},
	{ 0x00279cf8,   7},
	{ 0x00279d20,   2},
	{ 0x00279e40,  25},
	{ 0x00279ec8,   7},
	{ 0x00279eec,   1},
	{ 0x00279ef8,   7},
	{ 0x00279f20,   2},
	{ 0x0027a040,  25},
	{ 0x0027a0c8,   7},
	{ 0x0027a0ec,   1},
	{ 0x0027a0f8,   7},
	{ 0x0027a120,   2},
	{ 0x0027a240,  25},
	{ 0x0027a2c8,   7},
	{ 0x0027a2ec,   1},
	{ 0x0027a2f8,   7},
	{ 0x0027a320,   2},
	{ 0x0027a440,  25},
	{ 0x0027a4c8,   7},
	{ 0x0027a4ec,   1},
	{ 0x0027a4f8,   7},
	{ 0x0027a520,   2},
	{ 0x0027a640,  25},
	{ 0x0027a6c8,   7},
	{ 0x0027a6ec,   1},
	{ 0x0027a6f8,   7},
	{ 0x0027a720,   2},
	{ 0x0027a840,  25},
	{ 0x0027a8c8,   7},
	{ 0x0027a8ec,   1},
	{ 0x0027a8f8,   7},
	{ 0x0027a920,   2},
	{ 0x0027aa40,  25},
	{ 0x0027aac8,   7},
	{ 0x0027aaec,   1},
	{ 0x0027aaf8,   7},
	{ 0x0027ab20,   2},
	{ 0x0027ac40,  25},
	{ 0x0027acc8,   7},
	{ 0x0027acec,   1},
	{ 0x0027acf8,   7},
	{ 0x0027ad20,   2},
	{ 0x0027ae40,  25},
	{ 0x0027aec8,   7},
	{ 0x0027aeec,   1},
	{ 0x0027aef8,   7},
	{ 0x0027af20,   2},
	{ 0x0027b040,  25},
	{ 0x0027b0c8,   7},
	{ 0x0027b0ec,   1},
	{ 0x0027b0f8,   7},
	{ 0x0027b120,   2},
	{ 0x0027b240,  25},
	{ 0x0027b2c8,   7},
	{ 0x0027b2ec,   1},
	{ 0x0027b2f8,   7},
	{ 0x0027b320,   2},
	{ 0x0027b440,  25},
	{ 0x0027b4c8,   7},
	{ 0x0027b4ec,   1},
	{ 0x0027b4f8,   7},
	{ 0x0027b520,   2},
	{ 0x0027b640,  25},
	{ 0x0027b6c8,   7},
	{ 0x0027b6ec,   1},
	{ 0x0027b6f8,   7},
	{ 0x0027b720,   2},
	{ 0x0027b840,  25},
	{ 0x0027b8c8,   7},
	{ 0x0027b8ec,   1},
	{ 0x0027b8f8,   7},
	{ 0x0027b920,   2},
	{ 0x0027ba40,  25},
	{ 0x0027bac8,   7},
	{ 0x0027baec,   1},
	{ 0x0027baf8,   7},
	{ 0x0027bb20,   2},
	{ 0x0027bc40,  25},
	{ 0x0027bcc8,   7},
	{ 0x0027bcec,   1},
	{ 0x0027bcf8,   7},
	{ 0x0027bd20,   2},
	{ 0x0027be40,  25},
	{ 0x0027bec8,   7},
	{ 0x0027beec,   1},
	{ 0x0027bef8,   7},
	{ 0x0027bf20,   2},
	{ 0x0027c040,  25},
	{ 0x0027c0c8,   7},
	{ 0x0027c0ec,   1},
	{ 0x0027c0f8,   7},
	{ 0x0027c120,   2},
	{ 0x0027c240,  25},
	{ 0x0027c2c8,   7},
	{ 0x0027c2ec,   1},
	{ 0x0027c2f8,   7},
	{ 0x0027c320,   2},
	{ 0x0027c440,  25},
	{ 0x0027c4c8,   7},
	{ 0x0027c4ec,   1},
	{ 0x0027c4f8,   7},
	{ 0x0027c520,   2},
	{ 0x0027c640,  25},
	{ 0x0027c6c8,   7},
	{ 0x0027c6ec,   1},
	{ 0x0027c6f8,   7},
	{ 0x0027c720,   2},
	{ 0x0027c840,  25},
	{ 0x0027c8c8,   7},
	{ 0x0027c8ec,   1},
	{ 0x0027c8f8,   7},
	{ 0x0027c920,   2},
	{ 0x0027ca40,  25},
	{ 0x0027cac8,   7},
	{ 0x0027caec,   1},
	{ 0x0027caf8,   7},
	{ 0x0027cb20,   2},
	{ 0x0027cc40,  25},
	{ 0x0027ccc8,   7},
	{ 0x0027ccec,   1},
	{ 0x0027ccf8,   7},
	{ 0x0027cd20,   2},
	{ 0x0027ce40,  25},
	{ 0x0027cec8,   7},
	{ 0x0027ceec,   1},
	{ 0x0027cef8,   7},
	{ 0x0027cf20,   2},
	{ 0x0027d040,  25},
	{ 0x0027d0c8,   7},
	{ 0x0027d0ec,   1},
	{ 0x0027d0f8,   7},
	{ 0x0027d120,   2},
	{ 0x0027d240,  25},
	{ 0x0027d2c8,   7},
	{ 0x0027d2ec,   1},
	{ 0x0027d2f8,   7},
	{ 0x0027d320,   2},
	{ 0x0027d440,  25},
	{ 0x0027d4c8,   7},
	{ 0x0027d4ec,   1},
	{ 0x0027d4f8,   7},
	{ 0x0027d520,   2},
	{ 0x0027d640,  25},
	{ 0x0027d6c8,   7},
	{ 0x0027d6ec,   1},
	{ 0x0027d6f8,   7},
	{ 0x0027d720,   2},
	{ 0x0027d840,  25},
	{ 0x0027d8c8,   7},
	{ 0x0027d8ec,   1},
	{ 0x0027d8f8,   7},
	{ 0x0027d920,   2},
	{ 0x0027da40,  25},
	{ 0x0027dac8,   7},
	{ 0x0027daec,   1},
	{ 0x0027daf8,   7},
	{ 0x0027db20,   2},
	{ 0x0027dc40,  25},
	{ 0x0027dcc8,   7},
	{ 0x0027dcec,   1},
	{ 0x0027dcf8,   7},
	{ 0x0027dd20,   2},
	{ 0x0027de40,  25},
	{ 0x0027dec8,   7},
	{ 0x0027deec,   1},
	{ 0x0027def8,   7},
	{ 0x0027df20,   2},
	{ 0x0027e040,  25},
	{ 0x0027e0c8,   7},
	{ 0x0027e0ec,   1},
	{ 0x0027e0f8,   7},
	{ 0x0027e120,   2},
	{ 0x0027e240,  25},
	{ 0x0027e2c8,   7},
	{ 0x0027e2ec,   1},
	{ 0x0027e2f8,   7},
	{ 0x0027e320,   2},
	{ 0x0027e440,  25},
	{ 0x0027e4c8,   7},
	{ 0x0027e4ec,   1},
	{ 0x0027e4f8,   7},
	{ 0x0027e520,   2},
	{ 0x0027e640,  25},
	{ 0x0027e6c8,   7},
	{ 0x0027e6ec,   1},
	{ 0x0027e6f8,   7},
	{ 0x0027e720,   2},
	{ 0x0027e840,  25},
	{ 0x0027e8c8,   7},
	{ 0x0027e8ec,   1},
	{ 0x0027e8f8,   7},
	{ 0x0027e920,   2},
	{ 0x0027ea40,  25},
	{ 0x0027eac8,   7},
	{ 0x0027eaec,   1},
	{ 0x0027eaf8,   7},
	{ 0x0027eb20,   2},
	{ 0x0027ec40,  25},
	{ 0x0027ecc8,   7},
	{ 0x0027ecec,   1},
	{ 0x0027ecf8,   7},
	{ 0x0027ed20,   2},
	{ 0x0027ee40,  25},
	{ 0x0027eec8,   7},
	{ 0x0027eeec,   1},
	{ 0x0027eef8,   7},
	{ 0x0027ef20,   2},
	{ 0x0027f040,  25},
	{ 0x0027f0c8,   7},
	{ 0x0027f0ec,   1},
	{ 0x0027f0f8,   7},
	{ 0x0027f120,   2},
	{ 0x0027f240,  25},
	{ 0x0027f2c8,   7},
	{ 0x0027f2ec,   1},
	{ 0x0027f2f8,   7},
	{ 0x0027f320,   2},
	{ 0x0027f440,  25},
	{ 0x0027f4c8,   7},
	{ 0x0027f4ec,   1},
	{ 0x0027f4f8,   7},
	{ 0x0027f520,   2},
	{ 0x0027f640,  25},
	{ 0x0027f6c8,   7},
	{ 0x0027f6ec,   1},
	{ 0x0027f6f8,   7},
	{ 0x0027f720,   2},
	{ 0x0027f840,  25},
	{ 0x0027f8c8,   7},
	{ 0x0027f8ec,   1},
	{ 0x0027f8f8,   7},
	{ 0x0027f920,   2},
	{ 0x0027fa40,  25},
	{ 0x0027fac8,   7},
	{ 0x0027faec,   1},
	{ 0x0027faf8,   7},
	{ 0x0027fb20,   2},
	{ 0x0027fc40,  25},
	{ 0x0027fcc8,   7},
	{ 0x0027fcec,   1},
	{ 0x0027fcf8,   7},
	{ 0x0027fd20,   2},
	{ 0x0027fe40,  25},
	{ 0x0027fec8,   7},
	{ 0x0027feec,   1},
	{ 0x0027fef8,   7},
	{ 0x0027ff20,   2},
	{ 0x00400500,   1},
	{ 0x0040415c,   1},
	{ 0x00404468,   1},
	{ 0x00404498,   1},
	{ 0x00405800,   1},
	{ 0x00405840,   2},
	{ 0x00405850,   1},
	{ 0x00405908,   1},
	{ 0x00405a00,   1},
	{ 0x00405b50,   1},
	{ 0x00406024,   5},
	{ 0x00407010,   1},
	{ 0x00407808,   1},
	{ 0x0040803c,   1},
	{ 0x00408804,   1},
	{ 0x0040880c,   1},
	{ 0x00408900,   2},
	{ 0x00408910,   1},
	{ 0x00408944,   1},
	{ 0x00408984,   1},
	{ 0x004090a8,   1},
	{ 0x004098a0,   1},
	{ 0x00409b00,   1},
	{ 0x0041000c,   1},
	{ 0x00410110,   1},
	{ 0x00410184,   1},
	{ 0x0041040c,   1},
	{ 0x00410510,   1},
	{ 0x00410584,   1},
	{ 0x00418000,   1},
	{ 0x00418008,   1},
	{ 0x00418380,   2},
	{ 0x00418400,   2},
	{ 0x004184a0,   1},
	{ 0x00418604,   1},
	{ 0x00418680,   1},
	{ 0x00418704,   1},
	{ 0x00418714,   1},
	{ 0x00418800,   1},
	{ 0x0041881c,   1},
	{ 0x00418830,   1},
	{ 0x00418884,   1},
	{ 0x004188b0,   1},
	{ 0x004188c8,   3},
	{ 0x004188fc,   1},
	{ 0x00418b04,   1},
	{ 0x00418c04,   1},
	{ 0x00418c10,   8},
	{ 0x00418c88,   1},
	{ 0x00418d00,   1},
	{ 0x00418e00,   1},
	{ 0x00418e08,   1},
	{ 0x00418e34,   1},
	{ 0x00418e40,   4},
	{ 0x00418e58,  16},
	{ 0x00418f08,   1},
	{ 0x00419000,   1},
	{ 0x0041900c,   1},
	{ 0x00419018,   1},
	{ 0x00419854,   1},
	{ 0x00419864,   1},
	{ 0x00419a04,   2},
	{ 0x00419ab0,   1},
	{ 0x00419b04,   1},
	{ 0x00419b3c,   1},
	{ 0x00419b48,   1},
	{ 0x00419b50,   1},
	{ 0x00419ba0,   2},
	{ 0x00419bb0,   1},
	{ 0x00419bdc,   1},
	{ 0x00419c0c,   1},
	{ 0x00419d00,   1},
	{ 0x00419d08,   2},
	{ 0x00419e08,   1},
	{ 0x00419e80,   8},
	{ 0x00419ea8,   5},
	{ 0x00419f00,   8},
	{ 0x00419f28,   5},
	{ 0x00419f80,   8},
	{ 0x00419fa8,   5},
	{ 0x0041a02c,   2},
	{ 0x0041a0a8,   1},
	{ 0x0041a8a0,   3},
	{ 0x0041b014,   1},
	{ 0x0041b0cc,   1},
	{ 0x0041b1dc,   1},
	{ 0x0041b214,   1},
	{ 0x0041b2cc,   1},
	{ 0x0041b3dc,   1},
	{ 0x0041be0c,   3},
	{ 0x0041becc,   1},
	{ 0x0041bfdc,   1},
	{ 0x0041c054,   1},
	{ 0x0041c2b0,   1},
	{ 0x0041c304,   1},
	{ 0x0041c33c,   1},
	{ 0x0041c348,   1},
	{ 0x0041c350,   1},
	{ 0x0041c3a0,   2},
	{ 0x0041c3b0,   1},
	{ 0x0041c3dc,   1},
	{ 0x0041c40c,   1},
	{ 0x0041c500,   1},
	{ 0x0041c508,   2},
	{ 0x0041c608,   1},
	{ 0x0041c680,   8},
	{ 0x0041c6a8,   5},
	{ 0x0041c700,   8},
	{ 0x0041c728,   5},
	{ 0x0041c780,   8},
	{ 0x0041c7a8,   5},
	{ 0x0041c854,   1},
	{ 0x0041cab0,   1},
	{ 0x0041cb04,   1},
	{ 0x0041cb3c,   1},
	{ 0x0041cb48,   1},
	{ 0x0041cb50,   1},
	{ 0x0041cba0,   2},
	{ 0x0041cbb0,   1},
	{ 0x0041cbdc,   1},
	{ 0x0041cc0c,   1},
	{ 0x0041cd00,   1},
	{ 0x0041cd08,   2},
	{ 0x0041ce08,   1},
	{ 0x0041ce80,   8},
	{ 0x0041cea8,   5},
	{ 0x0041cf00,   8},
	{ 0x0041cf28,   5},
	{ 0x0041cf80,   8},
	{ 0x0041cfa8,   5},
	{ 0x0041d054,   1},
	{ 0x0041d2b0,   1},
	{ 0x0041d304,   1},
	{ 0x0041d33c,   1},
	{ 0x0041d348,   1},
	{ 0x0041d350,   1},
	{ 0x0041d3a0,   2},
	{ 0x0041d3b0,   1},
	{ 0x0041d3dc,   1},
	{ 0x0041d40c,   1},
	{ 0x0041d500,   1},
	{ 0x0041d508,   2},
	{ 0x0041d608,   1},
	{ 0x0041d680,   8},
	{ 0x0041d6a8,   5},
	{ 0x0041d700,   8},
	{ 0x0041d728,   5},
	{ 0x0041d780,   8},
	{ 0x0041d7a8,   5},
	{ 0x0041d854,   1},
	{ 0x0041dab0,   1},
	{ 0x0041db04,   1},
	{ 0x0041db3c,   1},
	{ 0x0041db48,   1},
	{ 0x0041db50,   1},
	{ 0x0041dba0,   2},
	{ 0x0041dbb0,   1},
	{ 0x0041dbdc,   1},
	{ 0x0041dc0c,   1},
	{ 0x0041dd00,   1},
	{ 0x0041dd08,   2},
	{ 0x0041de08,   1},
	{ 0x0041de80,   8},
	{ 0x0041dea8,   5},
	{ 0x0041df00,   8},
	{ 0x0041df28,   5},
	{ 0x0041df80,   8},
	{ 0x0041dfa8,   5},
	{ 0x00481a00,  19},
	{ 0x00481b00,  50},
	{ 0x00481e00,  50},
	{ 0x00481f00,  50},
	{ 0x00484200,  19},
	{ 0x00484300,  50},
	{ 0x00484600,  50},
	{ 0x00484700,  50},
	{ 0x00484a00,  19},
	{ 0x00484b00,  50},
	{ 0x00484e00,  50},
	{ 0x00484f00,  50},
	{ 0x00485200,  19},
	{ 0x00485300,  50},
	{ 0x00485600,  50},
	{ 0x00485700,  50},
	{ 0x00485a00,  19},
	{ 0x00485b00,  50},
	{ 0x00485e00,  50},
	{ 0x00485f00,  50},
	{ 0x00500384,   1},
	{ 0x005004a0,   1},
	{ 0x00500604,   1},
	{ 0x00500680,   1},
	{ 0x00500714,   1},
	{ 0x0050081c,   1},
	{ 0x00500884,   1},
	{ 0x005008b0,   1},
	{ 0x005008c8,   3},
	{ 0x005008fc,   1},
	{ 0x00500b04,   1},
	{ 0x00500c04,   1},
	{ 0x00500c10,   8},
	{ 0x00500c88,   1},
	{ 0x00500d00,   1},
	{ 0x00500e08,   1},
	{ 0x00500f08,   1},
	{ 0x00501000,   1},
	{ 0x0050100c,   1},
	{ 0x00501018,   1},
	{ 0x00501854,   1},
	{ 0x00501ab0,   1},
	{ 0x00501b04,   1},
	{ 0x00501b3c,   1},
	{ 0x00501b48,   1},
	{ 0x00501b50,   1},
	{ 0x00501ba0,   2},
	{ 0x00501bb0,   1},
	{ 0x00501bdc,   1},
	{ 0x00501c0c,   1},
	{ 0x00501d00,   1},
	{ 0x00501d08,   2},
	{ 0x00501e08,   1},
	{ 0x00501e80,   8},
	{ 0x00501ea8,   5},
	{ 0x00501f00,   8},
	{ 0x00501f28,   5},
	{ 0x00501f80,   8},
	{ 0x00501fa8,   5},
	{ 0x0050202c,   2},
	{ 0x005020a8,   1},
	{ 0x005028a0,   3},
	{ 0x00503014,   1},
	{ 0x005030cc,   1},
	{ 0x005031dc,   1},
	{ 0x00503214,   1},
	{ 0x005032cc,   1},
	{ 0x005033dc,   1},
	{ 0x00503e14,   1},
	{ 0x00503ecc,   1},
	{ 0x00503fdc,   1},
	{ 0x00504054,   1},
	{ 0x005042b0,   1},
	{ 0x00504304,   1},
	{ 0x0050433c,   1},
	{ 0x00504348,   1},
	{ 0x00504350,   1},
	{ 0x005043a0,   2},
	{ 0x005043b0,   1},
	{ 0x005043dc,   1},
	{ 0x0050440c,   1},
	{ 0x00504500,   1},
	{ 0x00504508,   2},
	{ 0x00504608,   1},
	{ 0x00504680,   8},
	{ 0x005046a8,   5},
	{ 0x00504700,   8},
	{ 0x00504728,   5},
	{ 0x00504780,   8},
	{ 0x005047a8,   5},
	{ 0x00504854,   1},
	{ 0x00504ab0,   1},
	{ 0x00504b04,   1},
	{ 0x00504b3c,   1},
	{ 0x00504b48,   1},
	{ 0x00504b50,   1},
	{ 0x00504ba0,   2},
	{ 0x00504bb0,   1},
	{ 0x00504bdc,   1},
	{ 0x00504c0c,   1},
	{ 0x00504d00,   1},
	{ 0x00504d08,   2},
	{ 0x00504e08,   1},
	{ 0x00504e80,   8},
	{ 0x00504ea8,   5},
	{ 0x00504f00,   8},
	{ 0x00504f28,   5},
	{ 0x00504f80,   8},
	{ 0x00504fa8,   5},
	{ 0x00505054,   1},
	{ 0x005052b0,   1},
	{ 0x00505304,   1},
	{ 0x0050533c,   1},
	{ 0x00505348,   1},
	{ 0x00505350,   1},
	{ 0x005053a0,   2},
	{ 0x005053b0,   1},
	{ 0x005053dc,   1},
	{ 0x0050540c,   1},
	{ 0x00505500,   1},
	{ 0x00505508,   2},
	{ 0x00505608,   1},
	{ 0x00505680,   8},
	{ 0x005056a8,   5},
	{ 0x00505700,   8},
	{ 0x00505728,   5},
	{ 0x00505780,   8},
	{ 0x005057a8,   5},
	{ 0x00505854,   1},
	{ 0x00505ab0,   1},
	{ 0x00505b04,   1},
	{ 0x00505b3c,   1},
	{ 0x00505b48,   1},
	{ 0x00505b50,   1},
	{ 0x00505ba0,   2},
	{ 0x00505bb0,   1},
	{ 0x00505bdc,   1},
	{ 0x00505c0c,   1},
	{ 0x00505d00,   1},
	{ 0x00505d08,   2},
	{ 0x00505e08,   1},
	{ 0x00505e80,   8},
	{ 0x00505ea8,   5},
	{ 0x00505f00,   8},
	{ 0x00505f28,   5},
	{ 0x00505f80,   8},
	{ 0x00505fa8,   5},
	{ 0x00581a00,  19},
	{ 0x00581b00,  50},
	{ 0x00581e00,  50},
	{ 0x00581f00,  50},
	{ 0x00584200,  19},
	{ 0x00584300,  50},
	{ 0x00584600,  50},
	{ 0x00584700,  50},
	{ 0x00584a00,  19},
	{ 0x00584b00,  50},
	{ 0x00584e00,  50},
	{ 0x00584f00,  50},
	{ 0x00585200,  19},
	{ 0x00585300,  50},
	{ 0x00585600,  50},
	{ 0x00585700,  50},
	{ 0x00585a00,  19},
	{ 0x00585b00,  50},
	{ 0x00585e00,  50},
	{ 0x00585f00,  50},
	{ 0x00900100,   1},
	{ 0x009a0100,   1},
	{ 0x00a00160,   2},
	{ 0x00a007d0,   1},
	{ 0x00a04200,   1},
	{ 0x00a04470,   2},
	{ 0x00a08190,   1},
	{ 0x00a08198,   4},
	{ 0x00a0c820,   2},
	{ 0x00a0cc20,   2},
	{ 0x00a0e470,   2},
	{ 0x00a0e490,   9},
	{ 0x00a0e6a8,   7},
	{ 0x00a0e6c8,   2},
	{ 0x00a0e6d4,   7},
	{ 0x00a0e6f4,   2},
	{ 0x00a0ec70,   2},
	{ 0x00a0ec90,   9},
	{ 0x00a0eea8,   7},
	{ 0x00a0eec8,   2},
	{ 0x00a0eed4,   7},
	{ 0x00a0eef4,   2},
	{ 0x00a10190,   1},
	{ 0x00a10198,   4},
	{ 0x00a14820,   2},
	{ 0x00a14c20,   2},
	{ 0x00a16470,   2},
	{ 0x00a16490,   9},
	{ 0x00a166a8,   7},
	{ 0x00a166c8,   2},
	{ 0x00a166d4,   7},
	{ 0x00a166f4,   2},
	{ 0x00a16c70,   2},
	{ 0x00a16c90,   9},
	{ 0x00a16ea8,   7},
	{ 0x00a16ec8,   2},
	{ 0x00a16ed4,   7},
	{ 0x00a16ef4,   2},
	{ 0x00a18190,   1},
	{ 0x00a18198,   4},
	{ 0x00a1c820,   2},
	{ 0x00a1cc20,   2},
	{ 0x00a1e470,   2},
	{ 0x00a1e490,   9},
	{ 0x00a1e6a8,   7},
	{ 0x00a1e6c8,   2},
	{ 0x00a1e6d4,   7},
	{ 0x00a1e6f4,   2},
	{ 0x00a1ec70,   2},
	{ 0x00a1ec90,   9},
	{ 0x00a1eea8,   7},
	{ 0x00a1eec8,   2},
	{ 0x00a1eed4,   7},
	{ 0x00a1eef4,   2},
	{ 0x00a20190,   1},
	{ 0x00a20198,   4},
	{ 0x00a24820,   2},
	{ 0x00a24c20,   2},
	{ 0x00a26470,   2},
	{ 0x00a26490,   9},
	{ 0x00a266a8,   7},
	{ 0x00a266c8,   2},
	{ 0x00a266d4,   7},
	{ 0x00a266f4,   2},
	{ 0x00a26c70,   2},
	{ 0x00a26c90,   9},
	{ 0x00a26ea8,   7},
	{ 0x00a26ec8,   2},
	{ 0x00a26ed4,   7},
	{ 0x00a26ef4,   2},
	{ 0x00a28190,   1},
	{ 0x00a28198,   4},
	{ 0x00a2c820,   2},
	{ 0x00a2cc20,   2},
	{ 0x00a2e470,   2},
	{ 0x00a2e490,   9},
	{ 0x00a2e6a8,   7},
	{ 0x00a2e6c8,   2},
	{ 0x00a2e6d4,   7},
	{ 0x00a2e6f4,   2},
	{ 0x00a2ec70,   2},
	{ 0x00a2ec90,   9},
	{ 0x00a2eea8,   7},
	{ 0x00a2eec8,   2},
	{ 0x00a2eed4,   7},
	{ 0x00a2eef4,   2},
	{ 0x00a30190,   1},
	{ 0x00a30198,   4},
	{ 0x00a34820,   2},
	{ 0x00a34c20,   2},
	{ 0x00a36470,   2},
	{ 0x00a36490,   9},
	{ 0x00a366a8,   7},
	{ 0x00a366c8,   2},
	{ 0x00a366d4,   7},
	{ 0x00a366f4,   2},
	{ 0x00a36c70,   2},
	{ 0x00a36c90,   9},
	{ 0x00a36ea8,   7},
	{ 0x00a36ec8,   2},
	{ 0x00a36ed4,   7},
	{ 0x00a36ef4,   2},
	{ 0x00a38190,   1},
	{ 0x00a38198,   4},
	{ 0x00a3c820,   2},
	{ 0x00a3cc20,   2},
	{ 0x00a3e470,   2},
	{ 0x00a3e490,   9},
	{ 0x00a3e6a8,   7},
	{ 0x00a3e6c8,   2},
	{ 0x00a3e6d4,   7},
	{ 0x00a3e6f4,   2},
	{ 0x00a3ec70,   2},
	{ 0x00a3ec90,   9},
	{ 0x00a3eea8,   7},
	{ 0x00a3eec8,   2},
	{ 0x00a3eed4,   7},
	{ 0x00a3eef4,   2},
};


static const u32 gv11b_global_whitelist_ranges_count =
	ARRAY_SIZE(gv11b_global_whitelist_ranges);

/* context */

/* runcontrol */
static const u32 gv11b_runcontrol_whitelist[] = {
};
static const u32 gv11b_runcontrol_whitelist_count =
	ARRAY_SIZE(gv11b_runcontrol_whitelist);

static const struct regop_offset_range gv11b_runcontrol_whitelist_ranges[] = {
};
static const u32 gv11b_runcontrol_whitelist_ranges_count =
	ARRAY_SIZE(gv11b_runcontrol_whitelist_ranges);


/* quad ctl */
static const u32 gv11b_qctl_whitelist[] = {
};
static const u32 gv11b_qctl_whitelist_count =
	ARRAY_SIZE(gv11b_qctl_whitelist);

static const struct regop_offset_range gv11b_qctl_whitelist_ranges[] = {
};
static const u32 gv11b_qctl_whitelist_ranges_count =
	ARRAY_SIZE(gv11b_qctl_whitelist_ranges);

const struct regop_offset_range *gv11b_get_global_whitelist_ranges(void)
{
	return gv11b_global_whitelist_ranges;
}

int gv11b_get_global_whitelist_ranges_count(void)
{
	return gv11b_global_whitelist_ranges_count;
}

const struct regop_offset_range *gv11b_get_context_whitelist_ranges(void)
{
	return gv11b_global_whitelist_ranges;
}

int gv11b_get_context_whitelist_ranges_count(void)
{
	return gv11b_global_whitelist_ranges_count;
}

const u32 *gv11b_get_runcontrol_whitelist(void)
{
	return gv11b_runcontrol_whitelist;
}

int gv11b_get_runcontrol_whitelist_count(void)
{
	return gv11b_runcontrol_whitelist_count;
}

const struct regop_offset_range *gv11b_get_runcontrol_whitelist_ranges(void)
{
	return gv11b_runcontrol_whitelist_ranges;
}

int gv11b_get_runcontrol_whitelist_ranges_count(void)
{
	return gv11b_runcontrol_whitelist_ranges_count;
}

const u32 *gv11b_get_qctl_whitelist(void)
{
	return gv11b_qctl_whitelist;
}

int gv11b_get_qctl_whitelist_count(void)
{
	return gv11b_qctl_whitelist_count;
}

const struct regop_offset_range *gv11b_get_qctl_whitelist_ranges(void)
{
	return gv11b_qctl_whitelist_ranges;
}

int gv11b_get_qctl_whitelist_ranges_count(void)
{
	return gv11b_qctl_whitelist_ranges_count;
}

int gv11b_apply_smpc_war(struct dbg_session_gk20a *dbg_s)
{
	/* Not needed on gv11b */
	return 0;
}
