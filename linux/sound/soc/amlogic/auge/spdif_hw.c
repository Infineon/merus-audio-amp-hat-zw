/*
 * sound/soc/amlogic/auge/spdif_hw.c
 *
 * Copyright (C) 2017 Amlogic, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */

#include <sound/soc.h>

#include "iomap.h"
#include "spdif_hw.h"

void aml_spdif_enable(
	struct aml_audio_controller *actrl,
	int stream,
	bool is_enable)
{
	pr_info("spdif stream :%d is_enable:%d\n", stream, is_enable);

	if (stream == SNDRV_PCM_STREAM_PLAYBACK) {
		aml_audiobus_update_bits(actrl,
			EE_AUDIO_SPDIFOUT_CTRL0, 1<<31, is_enable<<31);
	} else {
		aml_audiobus_update_bits(actrl,
			EE_AUDIO_SPDIFIN_CTRL0, 1<<31, is_enable<<31);
	}
}

void aml_spdif_arb_config(struct aml_audio_controller *actrl)
{
	/* config ddr arb */
	aml_audiobus_write(actrl, EE_AUDIO_ARB_CTRL, 1<<31|0xff<<0);
}

int aml_spdifin_status_check(struct aml_audio_controller *actrl)
{
	unsigned int val;

	val = aml_audiobus_read(actrl,
		EE_AUDIO_SPDIFIN_STAT0);

	/* pr_info("\t--- spdif handles status0 %#x\n", val); */
	return val;
}

void aml_spdifin_clr_irq(struct aml_audio_controller *actrl)
{
	aml_audiobus_update_bits(actrl,
			EE_AUDIO_SPDIFIN_CTRL0,
			1<<26,
			1<<26);
	aml_audiobus_update_bits(actrl,
			EE_AUDIO_SPDIFIN_CTRL0,
			1<<26,
			0);
}

void aml_spdif_fifo_reset(
	struct aml_audio_controller *actrl,
	int stream)
{
	if (stream == SNDRV_PCM_STREAM_PLAYBACK) {
		/* reset afifo */
		aml_audiobus_update_bits(actrl,
				EE_AUDIO_SPDIFOUT_CTRL0, 3<<28, 0);
		aml_audiobus_update_bits(actrl,
				EE_AUDIO_SPDIFOUT_CTRL0, 1<<29, 1<<29);
		aml_audiobus_update_bits(actrl,
				EE_AUDIO_SPDIFOUT_CTRL0, 1<<28, 1<<28);
	} else {
		/* reset afifo */
		aml_audiobus_update_bits(actrl,
				EE_AUDIO_SPDIFIN_CTRL0, 3<<28, 0);
		aml_audiobus_update_bits(actrl,
				EE_AUDIO_SPDIFIN_CTRL0, 1<<29, 1<<29);
		aml_audiobus_update_bits(actrl,
				EE_AUDIO_SPDIFIN_CTRL0, 1<<28, 1<<28);
	}
}

void aml_spdif_fifo_ctrl(
	struct aml_audio_controller *actrl,
	int bitwidth,
	int stream,
	unsigned int fifo_id)
{
	unsigned int frddr_type, toddr_type;

	switch (bitwidth) {
	case 8:
		frddr_type = 0;
		toddr_type = 0;
		break;
	case 16:
		frddr_type = 1;
		toddr_type = 1;
		break;
	case 24:
		frddr_type = 4;
		toddr_type = 4;
		break;
	case 32:
		frddr_type = 3;
		toddr_type = 3;
		break;
	default:
		pr_err("runtime format invalid bitwidth: %d\n",
			bitwidth);
		return;
	}

	pr_info("%s, bit depth:%d, frddr type:%d, toddr:type:%d\n",
	__func__, bitwidth, frddr_type, toddr_type);

	if (stream == SNDRV_PCM_STREAM_PLAYBACK) {
		// mask lane 0 L/R channels
		aml_audiobus_update_bits(actrl,
			EE_AUDIO_SPDIFOUT_CTRL0,
			0x1<<29|0x1<<28|0x1<<20|0x1<<19|0xff<<4,
			1<<29|1<<28|0<<20|0<<19|0x3<<4);

		aml_audiobus_update_bits(actrl,
			EE_AUDIO_SPDIFOUT_CTRL1,
			0x3 << 24 | 0x1f << 8 | 0x7 << 4,
			fifo_id << 24 | (bitwidth - 1) << 8 | frddr_type<<4);

		aml_audiobus_write(actrl,
			EE_AUDIO_SPDIFOUT_SWAP,
			1<<4);
	} else {
		unsigned int lsb;
		unsigned int spdifin_clk = 250000000 * 2;

		/* sysclk/rate/32(bit)/2(ch)/2(bmc) */
		unsigned int counter_32k  = (spdifin_clk / (32000  * 64));
		unsigned int counter_44k  = (spdifin_clk / (44100  * 64));
		unsigned int counter_48k  = (spdifin_clk / (48000  * 64));
		unsigned int counter_88k  = (spdifin_clk / (88200  * 64));
		unsigned int counter_96k  = (spdifin_clk / (96000  * 64));
		unsigned int counter_176k = (spdifin_clk / (176400 * 64));
		unsigned int counter_192k = (spdifin_clk / (192000 * 64));
		unsigned int mode0_th = 3 * (counter_32k + counter_44k) >> 1;
		unsigned int mode1_th = 3 * (counter_44k + counter_48k) >> 1;
		unsigned int mode2_th = 3 * (counter_48k + counter_88k) >> 1;
		unsigned int mode3_th = 3 * (counter_88k + counter_96k) >> 1;
		unsigned int mode4_th = 3 * (counter_96k + counter_176k) >> 1;
		unsigned int mode5_th = 3 * (counter_176k + counter_192k) >> 1;
		unsigned int mode0_timer = counter_32k >> 1;
		unsigned int mode1_timer = counter_44k >> 1;
		unsigned int mode2_timer = counter_48k >> 1;
		unsigned int mode3_timer = counter_88k >> 1;
		unsigned int mode4_timer = counter_96k >> 1;
		unsigned int mode5_timer = counter_176k >> 1;
		unsigned int mode6_timer = counter_192k >> 1;

		if (bitwidth <= 24)
			lsb = 28 - bitwidth;
		else
			lsb = 4;

		// 250M
		aml_audiobus_write(actrl,
			EE_AUDIO_SPDIFIN_CTRL1,
			0xff << 20 | 25000 << 0);
#if 1
		aml_audiobus_write(actrl,
			EE_AUDIO_SPDIFIN_CTRL2,
			mode0_th << 20 |
			mode1_th << 10 |
			mode2_th << 0);

		aml_audiobus_write(actrl,
			EE_AUDIO_SPDIFIN_CTRL3,
			mode3_th << 20 |
			mode4_th << 10 |
			mode5_th << 0);

		aml_audiobus_write(actrl,
			EE_AUDIO_SPDIFIN_CTRL4,
			(mode0_timer << 24) |
			(mode1_timer << 16) |
			(mode2_timer << 8)  |
			(mode3_timer << 0)
			);

		aml_audiobus_write(actrl,
			EE_AUDIO_SPDIFIN_CTRL5,
			(mode4_timer << 24) |
			(mode5_timer << 16) |
			(mode6_timer << 8)
			);
#else
		aml_audiobus_write(actrl,
			EE_AUDIO_SPDIFIN_CTRL2,
			140 << 20 | 100 << 10 | 86 << 0);

		aml_audiobus_write(actrl,
			EE_AUDIO_SPDIFIN_CTRL3,
			83 << 20 | 60 << 10 | 30 << 0);

		aml_audiobus_write(actrl,
			EE_AUDIO_SPDIFIN_CTRL4,
			(81<<24) | /* reg_sample_mode0_timer */
			(61<<16) | /* reg_sample_mode1_timer */
			(44<<8) | /* reg_sample_mode2_timer*/
			(42<<0)
			);

		aml_audiobus_write(actrl,
			EE_AUDIO_SPDIFIN_CTRL5,
			(40<<24) | /* reg_sample_mode4_timer	  = 5[31:24]; */
			(20<<16) | /* reg_sample_mode5_timer	  = 5[23:16]; */
			(9<<8) |  /* reg_sample_mode6_timer   = 5[15:8]; */
			(0<<0)	   /* reg_sample_mode7_timer	  = 5[7:0]; */
			);
#endif
		aml_audiobus_update_bits(actrl,
			EE_AUDIO_SPDIFIN_CTRL0,
			0x1<<25|0x1<<24|1<<12,
			0x1<<25|0x0<<24|1<<12);
	}

}

int spdifin_get_mode(void)
{
	int mode_val = audiobus_read(EE_AUDIO_SPDIFIN_STAT0);

	mode_val >>= 28;
	mode_val &= 0x7;

	return mode_val;
}

int spdif_get_channel_status(int reg)
{
	return audiobus_read(reg);
}

void spdifin_set_channel_status(int ch, int bits)
{
	int ch_status_sel = (ch << 3 | bits) & 0xf;

	/*which channel status would be got*/
	audiobus_update_bits(EE_AUDIO_SPDIFIN_CTRL0,
		0xf << 8,
		ch_status_sel << 8);
}
int spdifin_get_sample_rate(void)
{
	unsigned int val;

	val = audiobus_read(EE_AUDIO_SPDIFIN_STAT0);
	if (((val >> 18) & 0x3ff) == 0x3ff) //N/A
		return 7;

	return (val >> 28) & 0x7;
}

static int spdifin_get_channel_status(int sel)
{
	unsigned int val;

	/* set ch_status_sel to channel status */
	audiobus_update_bits(EE_AUDIO_SPDIFIN_CTRL0, 0xf << 8, sel << 8);

	val = audiobus_read(EE_AUDIO_SPDIFIN_STAT1);

	return val;
}

int spdifin_get_ch_status0to31(void)
{
	return spdifin_get_channel_status(0x0);
}

int spdifin_get_audio_type(void)
{
	unsigned int val;

	/* set ch_status_sel to read Pc */
	val = spdifin_get_channel_status(0x6);

	return (val >> 16) & 0xff;
}
