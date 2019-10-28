/*
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Author York Sun <york.sun@nxp.com>
 */

#include <platform_def.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <debug.h>
#include <errno.h>
#include <string.h>
#include <io.h>
#include <ddr.h>
#include <utils.h>
#include <utils_def.h>
#include <errata.h>

const struct ddr_cfg_regs static_1500 = {
	.cs[0].bnds = 0xFF,
	.cs[0].config = 0x80010322,
	.cs[0].config_2 = 0x00,
	.timing_cfg[0] = 0xDA550018,
	.timing_cfg[1] = 0xBBB48E34,
	.timing_cfg[2] = 0x00491111,
	.timing_cfg[3] = 0x010C1000,
	.timing_cfg[4] = 0x00220001,
	.timing_cfg[5] = 0x03401400,
	.timing_cfg[7] = 0x13340000,
	.timing_cfg[8] = 0x00335800,
	.sdram_cfg[0] = 0x45200000,
	.sdram_cfg[1] = 0x00401070,
	.sdram_cfg[2] = 0x00,
	.sdram_mode[0] = 0x01010211,
	.sdram_mode[1] = 0x00,
	.sdram_mode[2] = 0x00,
	.sdram_mode[3] = 0x00,
	.sdram_mode[4] = 0x00,
	.sdram_mode[5] = 0x00,
	.sdram_mode[6] = 0x00,
	.sdram_mode[7] = 0x00,
	.sdram_mode[8] = 0x0701,
	.sdram_mode[9] = 0x04800000,
	.sdram_mode[10] = 0x00,
	.sdram_mode[11] = 0x00,
	.sdram_mode[12] = 0x00,
	.sdram_mode[13] = 0x00,
	.sdram_mode[14] = 0x00,
	.sdram_mode[15] = 0x00,
	.md_cntl = 0x00,
	.interval = 0x18600618,
	.data_init = 0xDEADBEEF,
	.clk_cntl = 0x02800000,
	.init_addr = 0x00,
	.init_ext_addr = 0x00,
	.zq_cntl = 0x8A090705,
	.wrlvl_cntl[0] = 0x8655F609,
	.wrlvl_cntl[1] = 0x00,
	.wrlvl_cntl[2] = 0x00,
	.sdram_rcw[0] = 0x00,
	.sdram_rcw[1] = 0x00,
	.sdram_rcw[2] = 0x00,
	.sdram_rcw[3] = 0x00,
	.sdram_rcw[4] = 0x00,
	.sdram_rcw[5] = 0x00,
	.cdr[0] = 0x80040000,
	.cdr[1] = 0x81,
	.err_disable = 0x00,
	.err_int_en = 0x1D,
	.debug[28] = 0x46, /* cpo_sample for A009942 */
};

long long board_static_ddr(struct ddr_info *priv)
{
	memcpy(&priv->ddr_reg, &static_1500, sizeof(struct ddr_cfg_regs));
	return 0x100000000UL;
}

long long _init_ddr(void)
{
	int spd_addr[] = { NXP_SPD_EEPROM0 };
	struct ddr_info info;
	struct sysinfo sys;
	long long dram_size;

	zeromem(&sys, sizeof(sys));
	get_clocks(&sys);
	debug("platform clock %lu\n", sys.freq_platform);
	debug("DDR PLL1 %lu\n", sys.freq_ddr_pll0);
	debug("DDR PLL2 %lu\n", sys.freq_ddr_pll1);

	zeromem(&info, sizeof(struct ddr_info));
	info.num_ctlrs = 1;
	info.dimm_on_ctlr = 1;
	info.clk = get_ddr_freq(&sys, 0);
	info.spd_addr = spd_addr;
	info.ddr[0] = (void *)NXP_DDR_ADDR;

	dram_size = dram_init(&info);

	if (dram_size < 0)
		ERROR("DDR init failed.\n");

	erratum_a008850_post();

	return dram_size;
}
