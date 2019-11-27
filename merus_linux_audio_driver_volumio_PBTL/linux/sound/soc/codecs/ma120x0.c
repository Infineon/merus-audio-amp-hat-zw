/*
 * ASoC Driver for ma120x0
 *
 * Author:	Ariel Muszkat <ariel.muszkat@infineon.com>
 *		Copyright 2019
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm_runtime.h>
#include <linux/i2c.h>
#include <linux/of_device.h>
#include <linux/spi/spi.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/tlv.h>
#include <linux/interrupt.h>

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include "ma120x0.h"

#define SOC_ENUM_ERR(xname, xenum) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname,\
	.access = SNDRV_CTL_ELEM_ACCESS_READ,\
	.info = snd_soc_info_enum_double, \
	.get = snd_soc_get_enum_double, .put = snd_soc_put_enum_double, \
	.private_value = (unsigned long)&xenum }


static struct i2c_client *i2c;

struct ma120x0_priv {
	struct regmap *regmap;
	int mclk_div;
	struct snd_soc_component *component;
	struct gpio_desc *enable_gpio;
	struct gpio_desc *mute_gpio;
	struct gpio_desc *booster_gpio;
	struct gpio_desc *msel_gpio;
	struct gpio_desc *error_gpio;

};

static struct ma120x0_priv *priv_data;

static unsigned int irqNumber;  ///< Used to share the IRQ number within this file

// Function prototype for the custom IRQ handler function -- see below for the implementation
//static irq_handler_t  ebbgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irqreturn_t ma120x0_irq_handler(int irq, void *data);

/*
 *    _   _    ___   _      ___         _           _
 *   /_\ | |  / __| /_\    / __|___ _ _| |_ _ _ ___| |___
 *  / _ \| |__\__ \/ _ \  | (__/ _ \ ' \  _| '_/ _ \ (_-<
 * /_/ \_\____|___/_/ \_\  \___\___/_||_\__|_| \___/_/__/
 *
 */


static const char * const limEnable_text[] = {"Bypassed", "Enabled"};
static const char * const limAttack_text[] = {"Slow", "Normal", "Fast"};
static const char * const limRelease_text[] = {"Slow", "Normal", "Fast"};
//static const char * const audioproc_mute_text[] = {"Play", "Mute"};

static const char * const err_flycap_text[] = {"Ok", "Error"};
static const char * const err_overcurr_text[] = {"Ok", "Error"};
static const char * const err_pllerr_text[] = {"Ok", "Error"};
static const char * const err_pvddunder_text[] = {"Ok", "Error"};
static const char * const err_overtempw_text[] = {"Ok", "Error"};
static const char * const err_overtempe_text[] = {"Ok", "Error"};
static const char * const err_pinlowimp_text[] = {"Ok", "Error"};
static const char * const err_dcprot_text[] = {"Ok", "Error"};

static const char * const pwr_mode_prof_text[] = {"PMF0", "PMF1", "PMF2", "PMF3", "PMF4"};
//static const char * const pwr_mode_control_text[] = {"Automatic", "Manual"};

static const struct soc_enum limEnable_ctrl =
	SOC_ENUM_SINGLE(MA_audio_proc_limiterEnable__a, MA_audio_proc_limiterEnable__shift,
		MA_audio_proc_limiterEnable__len + 1, limEnable_text);
static const struct soc_enum limAttack_ctrl =
	SOC_ENUM_SINGLE(MA_audio_proc_attack__a, MA_audio_proc_attack__shift,
		MA_audio_proc_attack__len + 1, limAttack_text);
static const struct soc_enum limRelease_ctrl =
	SOC_ENUM_SINGLE(MA_audio_proc_release__a, MA_audio_proc_release__shift,
		MA_audio_proc_release__len + 1, limRelease_text);
//static const struct soc_enum audioproc_mute_ctrl =
	//SOC_ENUM_SINGLE(MA_audio_proc_mute__a, MA_audio_proc_mute__shift,
		//MA_audio_proc_mute__len + 1, audioproc_mute_text);

static const struct soc_enum err_flycap_ctrl =
	SOC_ENUM_SINGLE(MA_error__a, 0, 3, err_flycap_text);
static const struct soc_enum err_overcurr_ctrl =
	SOC_ENUM_SINGLE(MA_error__a, 1, 3, err_overcurr_text);
static const struct soc_enum err_pllerr_ctrl =
	SOC_ENUM_SINGLE(MA_error__a, 2, 3, err_pllerr_text);
static const struct soc_enum err_pvddunder_ctrl =
	SOC_ENUM_SINGLE(MA_error__a, 3, 3, err_pvddunder_text);
static const struct soc_enum err_overtempw_ctrl =
	SOC_ENUM_SINGLE(MA_error__a, 4, 3, err_overtempw_text);
static const struct soc_enum err_overtempe_ctrl =
	SOC_ENUM_SINGLE(MA_error__a, 5, 3, err_overtempe_text);
static const struct soc_enum err_pinlowimp_ctrl =
	SOC_ENUM_SINGLE(MA_error__a, 6, 3, err_pinlowimp_text);
static const struct soc_enum err_dcprot_ctrl =
	SOC_ENUM_SINGLE(MA_error__a, 7, 3, err_dcprot_text);

static const struct soc_enum pwr_mode_prof_ctrl =
	SOC_ENUM_SINGLE(MA_PMprofile__a, MA_PMprofile__shift, 5, pwr_mode_prof_text);

//static const struct soc_enum pwr_mode_control_ctrl =
	//SOC_ENUM_SINGLE(MA_manualPM__a, MA_manualPM__shift, MA_manualPM__len + 1, pwr_mode_control_text);

static const char * const pwr_mode_texts[] = {
		"Dynamic power mode",
		"Power mode 1",
		"Power mode 2",
		"Power mode 3",
	};

static const int pwr_mode_values[] = {
		0x10,
		0x50,
		0x60,
		0x70,
	};

static SOC_VALUE_ENUM_SINGLE_DECL(pwr_mode_ctrl,
					  MA_PM_man__a, 0, 0x70,
					  pwr_mode_texts,
					  pwr_mode_values);


static const DECLARE_TLV_DB_SCALE(ma120x0_vol_tlv, -5000, 100,  0);
static const DECLARE_TLV_DB_SCALE(ma120x0_lim_tlv, -5000, 100,  0);
static const DECLARE_TLV_DB_SCALE(ma120x0_lr_tlv, -5000, 100,  0);

static const struct snd_kcontrol_new ma120x0_snd_controls[] = {
	//Master Volume
	SOC_SINGLE_RANGE_TLV("A.Mstr Vol Volume", MA_vol_db_master__a, 0, 0x18, 0x4a, 1, ma120x0_vol_tlv),

	//L-R Volume ch0
	SOC_SINGLE_RANGE_TLV("B.L Vol Volume", MA_vol_db_ch0__a, 0, 0x18, 0x4a, 1, ma120x0_lr_tlv),
	SOC_SINGLE_RANGE_TLV("C.R Vol Volume", MA_vol_db_ch1__a, 0, 0x18, 0x4a, 1, ma120x0_lr_tlv),

	//L-R Limiter Treshold ch0-ch1
	SOC_DOUBLE_R_RANGE_TLV ("D.Lim tresh Volume"    , MA_thr_db_ch0__a, MA_thr_db_ch1__a, 0, 0x0e, 0x4a, 1, ma120x0_lim_tlv),

	//Enum Switches/Selectors
	//SOC_ENUM("E.AudioProc Mute", audioproc_mute_ctrl),
	SOC_ENUM("F.Limiter Enable", limEnable_ctrl),
	SOC_ENUM("G.Limiter Attck", limAttack_ctrl),
	SOC_ENUM("H.Limiter Rls", limRelease_ctrl),

	//Enum Error Monitor (read-only)
	SOC_ENUM_ERR("I.Err flycap", err_flycap_ctrl),
	SOC_ENUM_ERR("J.Err overcurr", err_overcurr_ctrl),
	SOC_ENUM_ERR("K.Err pllerr", err_pllerr_ctrl),
	SOC_ENUM_ERR("L.Err pvddunder", err_pvddunder_ctrl),
	SOC_ENUM_ERR("M.Err overtempw", err_overtempw_ctrl),
	SOC_ENUM_ERR("N.Err overtempe", err_overtempe_ctrl),
	SOC_ENUM_ERR("O.Err pinlowimp", err_pinlowimp_ctrl),
	SOC_ENUM_ERR("P.Err dcprot", err_dcprot_ctrl),

	//Power modes profiles
  SOC_ENUM("Q.PM Prof", pwr_mode_prof_ctrl),

	// Power mode control (automatic/manual)
	//SOC_ENUM("R.PM control", pwr_mode_control_ctrl),

	// Power mode selection (Dynamic,1,2,3)
	SOC_ENUM("R.Power Mode", pwr_mode_ctrl),


};


/*
 *  __  __         _    _            ___      _
 * |  \/  |__ _ __| |_ (_)_ _  ___  |   \ _ _(_)_ _____ _ _
 * | |\/| / _` / _| ' \| | ' \/ -_) | |) | '_| \ V / -_) '_|
 * |_|  |_\__,_\__|_||_|_|_||_\___| |___/|_| |_|\_/\___|_|
 *
 */

static int ma120x0_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params,
			    struct snd_soc_dai *dai)
{
	u16 blen = 0x00;

	struct snd_soc_component *component = dai->component;
	priv_data->component = component;

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
		blen = 0x10;
		break;
	case SNDRV_PCM_FORMAT_S24_LE: //(Only 4bytes 24bit format supppported on BCM2835 I2S Driver)
		blen = 0x00; //0x08;
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		blen = 0x00;
		break;
	default:
		dev_err(dai->dev, "Unsupported word length: %u\n",
			params_format(params));
		return -EINVAL;
	}

	// set word length
	snd_soc_component_update_bits(component, MA_i2s_framesize__a, MA_i2s_framesize__mask, blen);

	return 0;
}


static int ma120x0_mute_stream(struct snd_soc_dai *dai, int mute, int stream)
{
	int val = 0;

	struct ma120x0_priv *ma120x0;
	struct snd_soc_component *component = dai->component;
	ma120x0 = snd_soc_component_get_drvdata(component);

	if (mute)
		val = 0;
	else
		val = 1;

	gpiod_set_value_cansleep(priv_data->mute_gpio, val);

	return 0;
}

static int ma120x02_mute_stream(struct snd_soc_dai *dai, int mute, int stream)
{
	unsigned int val = 0;

	struct snd_soc_component *component = dai->component;
	priv_data->component = component;

	if (mute) {
		return snd_soc_component_write(component, MA_audio_proc_mute__a, 0xc1);
	} else {
		return snd_soc_component_write(component, MA_audio_proc_mute__a, 0x41);
	}

}

static const struct snd_soc_dai_ops ma120x0_dai_ops = {
	.hw_params 		= ma120x0_hw_params,
	.mute_stream	= ma120x02_mute_stream,
	//.digital_mute	= ma120x0_digital_mute,
};

static struct snd_soc_dai_driver ma120x0_dai = {
	.name		= "ma120x0-amp",
	.playback 	= {
		.stream_name	= "Playback",
		.channels_min	= 2,
		.channels_max	= 2,
		.rates = SNDRV_PCM_RATE_CONTINUOUS,
		.rate_min = 44100,
		.rate_max = 48000,
		.formats = SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S32_LE
	},
	.ops        = &ma120x0_dai_ops,
};


/*
 *   ___         _          ___      _
 *  / __|___  __| |___ __  |   \ _ _(_)_ _____ _ _
 * | (__/ _ \/ _` / -_) _| | |) | '_| \ V / -_) '_|
 *  \___\___/\__,_\___\__| |___/|_| |_|\_/\___|_|
 *
 */

 static int ma120x0_clear_err(struct snd_soc_component *component)
 {
	int ret = 0;

	struct ma120x0_priv *ma120x0;
	ma120x0 = snd_soc_component_get_drvdata(component);


	ret = snd_soc_component_update_bits(component, MA_eh_clear__a, MA_eh_clear__mask, 0x00);
	if (ret < 0) return ret;

	ret = snd_soc_component_update_bits(component, MA_eh_clear__a, MA_eh_clear__mask, 0x04);
	if (ret < 0) return ret;

	ret = snd_soc_component_update_bits(component, MA_eh_clear__a, MA_eh_clear__mask, 0x00);
	if (ret < 0) return ret;

	return 0;
}



static void ma120x0_remove(struct snd_soc_component *component)
{
	struct ma120x0_priv *ma120x0;

	ma120x0 = snd_soc_component_get_drvdata(component);
}


static int ma120x0_probe(struct snd_soc_component *component)
{
	struct ma120x0_priv *ma120x0;
	int ret = 0;

	i2c = container_of(component->dev, struct i2c_client, dev);

	ma120x0 = snd_soc_component_get_drvdata(component);

	//Reset error
	ma120x0_clear_err(component);
	if (ret < 0) return ret;

	// set serial audio format I2S and enable audio processor
	ret = snd_soc_component_write(component, MA_i2s_format__a, 0x08);
	if (ret < 0) return ret;

	//Mute (digital mute)
	ret = snd_soc_component_write(component, MA_audio_proc_mute__a, 0xc1);
	if (ret < 0) return ret;

	// Enable audio limiter
	ret = snd_soc_component_update_bits(component, MA_audio_proc_limiterEnable__a, MA_audio_proc_limiterEnable__mask, 0x40);
	if (ret < 0) return ret;

	// Set lim attack to fast
	ret = snd_soc_component_update_bits(component, MA_audio_proc_attack__a, MA_audio_proc_attack__mask, 0x80);
	if (ret < 0) return ret;

	// Set lim attack to low
	ret = snd_soc_component_update_bits(component, MA_audio_proc_release__a, MA_audio_proc_release__mask, 0x00);
	if (ret < 0) return ret;

	// set volume to -10dB
	ret = snd_soc_component_write(component, MA_vol_db_master__a, 0x12);
	if (ret < 0) return ret;

	// set ch0 lim tresh to -15dB
	ret = snd_soc_component_write(component, MA_thr_db_ch0__a, 0x27);
	if (ret < 0) return ret;

	// set ch1 lim tresh to -15dB
	ret = snd_soc_component_write(component, MA_thr_db_ch1__a, 0x27);
	if (ret < 0) return ret;

	//Check for errors
	ret = snd_soc_component_test_bits(component, MA_error_acc__a, 0x00, 0);
	if (ret < 0) return ret;
	ret = snd_soc_component_test_bits(component, MA_error_acc__a, 0x01, 0);
	if (ret < 0) return ret;
	ret = snd_soc_component_test_bits(component, MA_error_acc__a, 0x02, 0);
	if (ret < 0) return ret;
	ret = snd_soc_component_test_bits(component, MA_error_acc__a, 0x08, 0);
	if (ret < 0) return ret;
	ret = snd_soc_component_test_bits(component, MA_error_acc__a, 0x10, 0);
	if (ret < 0) return ret;
	ret = snd_soc_component_test_bits(component, MA_error_acc__a, 0x20, 0);
	if (ret < 0) return ret;
	ret = snd_soc_component_test_bits(component, MA_error_acc__a, 0x40, 0);
	if (ret < 0) return ret;
	ret = snd_soc_component_test_bits(component, MA_error_acc__a, 0x80, 0);
	if (ret < 0) return ret;

	return 0;
}

static const struct snd_soc_dapm_widget ma120x0_dapm_widgets[] = {
	SND_SOC_DAPM_OUTPUT("OUT_A"),
	SND_SOC_DAPM_OUTPUT("OUT_B"),
};

static const struct snd_soc_dapm_route ma120x0_dapm_routes[] = {
	{ "OUT_B",  NULL, "Playback" },
	{ "OUT_A",  NULL, "Playback" },
};


static const struct snd_soc_component_driver ma120x0_component_driver = {
	.probe = ma120x0_probe,
	.remove = ma120x0_remove,
	.dapm_widgets		= ma120x0_dapm_widgets,
	.num_dapm_widgets	= ARRAY_SIZE(ma120x0_dapm_widgets),
	.dapm_routes		= ma120x0_dapm_routes,
	.num_dapm_routes	= ARRAY_SIZE(ma120x0_dapm_routes),
	.controls = ma120x0_snd_controls,
	.num_controls = ARRAY_SIZE(ma120x0_snd_controls),
};



/*
 *   ___ ___ ___   ___      _
 *  |_ _|_  ) __| |   \ _ _(_)_ _____ _ _
 *   | | / / (__  | |) | '_| \ V / -_) '_|
 *  |___/___\___| |___/|_| |_|\_/\___|_|
 *
 */

static const struct reg_default ma120x0_reg_defaults[] = {
	{	0x01,	0x3c	},
};


static bool ma120x0_reg_volatile(struct device *dev, unsigned int reg)
{
	switch (reg) {
		case MA_error__a:
			return true;
	default:
			return false;
	}
}


static const struct of_device_id ma120x0_of_match[] = {
	{ .compatible = "ma,ma120x0", },
	{ }
};
MODULE_DEVICE_TABLE(of, ma120x0_of_match);


static struct regmap_config ma120x0_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,

	.max_register = 255,
	.volatile_reg = ma120x0_reg_volatile,

	.cache_type = REGCACHE_RBTREE,
	.reg_defaults = ma120x0_reg_defaults,
	.num_reg_defaults = ARRAY_SIZE(ma120x0_reg_defaults),
};
EXPORT_SYMBOL_GPL(ma120x0_regmap_config);

static int ma120x0_i2c_probe(struct i2c_client *i2c,
			    const struct i2c_device_id *id)
{
	int ret;

	priv_data = devm_kzalloc(&i2c->dev, sizeof *priv_data, GFP_KERNEL);
	if (!priv_data)
		return -ENOMEM;
	i2c_set_clientdata(i2c, priv_data);

	priv_data->regmap = devm_regmap_init_i2c(i2c, &ma120x0_regmap_config);
	if (IS_ERR(priv_data->regmap)) {
		ret = PTR_ERR(priv_data->regmap);
		return ret;
	}


	//Startup sequence

	//Make sure the device is muted
	priv_data->mute_gpio = devm_gpiod_get(&i2c->dev, "mute_gp",
						GPIOD_OUT_LOW);
	if (IS_ERR(priv_data->mute_gpio)) {
		ret = PTR_ERR(priv_data->mute_gpio);
		dev_err(&i2c->dev, "Failed to get mute gpio line: %d\n", ret);
		return ret;
	}
	msleep(50);

	//Make sure the booster  is not enabled  (acutally this should be in the
 // sound card driver. For sync and secure boot up pourposes is included here)
	priv_data->booster_gpio = devm_gpiod_get(&i2c->dev, "booster_gp",
						GPIOD_OUT_LOW);
	if (IS_ERR(priv_data->booster_gpio)) {
		ret = PTR_ERR(priv_data->booster_gpio);
		dev_err(&i2c->dev, "Failed to get booster enable gpio line: %d\n", ret);
		return ret;
	}
	msleep(50);


	//Enable booster and wait 200ms until stable PVDD
	gpiod_set_value_cansleep(priv_data->booster_gpio, 1);
	msleep(200);

  
	priv_data->msel_gpio = devm_gpiod_get(&i2c->dev, "msel_gp",
						GPIOD_OUT_LOW);
	if (IS_ERR(priv_data->msel_gpio)) {
		ret = PTR_ERR(priv_data->msel_gpio);
		dev_err(&i2c->dev, "Failed to get msel gpio line: %d\n", ret);
		return ret;
	}
	msleep(100);


	//Enable ma120x0p
	priv_data->enable_gpio = devm_gpiod_get(&i2c->dev, "enable_gp",
						GPIOD_OUT_LOW);
	if (IS_ERR(priv_data->enable_gpio)) {
		ret = PTR_ERR(priv_data->enable_gpio);
		dev_err(&i2c->dev, "Failed to get ma120x0 enable gpio line: %d\n", ret);
		return ret;
	}
	msleep(50);

	//Unmute
	gpiod_set_value_cansleep(priv_data->mute_gpio, 1);

	//Get error input gpio ma120x0p
	priv_data->error_gpio = devm_gpiod_get(&i2c->dev, "error_gp",
						GPIOD_IN);
	if (IS_ERR(priv_data->error_gpio)) {
		ret = PTR_ERR(priv_data->error_gpio);
		dev_err(&i2c->dev, "Failed to get ma120x0 error gpio line: %d\n", ret);
		return ret;
	}

	irqNumber = gpiod_to_irq(priv_data->error_gpio);
   printk(KERN_INFO "GPIO_TEST: The button is mapped to IRQ: %d\n", irqNumber);

	 ret = devm_request_threaded_irq(&i2c->dev, irqNumber, ma120x0_irq_handler,\
	 			NULL, IRQF_TRIGGER_FALLING,\
	 			"ma120x0", priv_data);
		if (ret != 0) {
			dev_warn(&i2c->dev, "Failed to request IRQ: %d\n", ret);
		} else {
			printk(KERN_INFO "GPIO_TEST: The interrupt request result is: %d\n", ret);
		}

	ret = devm_snd_soc_register_component(&i2c->dev,
				     &ma120x0_component_driver, &ma120x0_dai, 1);

	return ret;
}
EXPORT_SYMBOL_GPL(ma120x0_i2c_probe);

static irqreturn_t ma120x0_irq_handler(int irq, void *data)
{
	gpiod_set_value_cansleep(priv_data->mute_gpio, 0);
	gpiod_set_value_cansleep(priv_data->enable_gpio, 1);
	return IRQ_HANDLED;
}


static int ma120x0_i2c_remove(struct i2c_client *i2c)
{
	snd_soc_unregister_component(&i2c->dev);
	i2c_set_clientdata(i2c, NULL);

	gpiod_set_value_cansleep(priv_data->mute_gpio, 0);
	msleep(30);
	gpiod_set_value_cansleep(priv_data->enable_gpio, 1);
	msleep(200);
	gpiod_set_value_cansleep(priv_data->booster_gpio, 0);
	msleep(200);

	kfree(priv_data);

	return 0;
}
EXPORT_SYMBOL_GPL(ma120x0_i2c_remove);

static void ma120x0_i2c_shutdown(struct i2c_client *i2c)
{
	snd_soc_unregister_component(&i2c->dev);
	i2c_set_clientdata(i2c, NULL);

	gpiod_set_value_cansleep(priv_data->mute_gpio, 0);
	msleep(30);
	gpiod_set_value_cansleep(priv_data->enable_gpio, 1);
	msleep(200);
	gpiod_set_value_cansleep(priv_data->booster_gpio, 0);
	msleep(200);

	kfree(priv_data);

}
EXPORT_SYMBOL_GPL(ma120x0_i2c_shutdown);


static const struct i2c_device_id ma120x0_i2c_id[] = {
	{ "ma120x0", 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, ma120x0_i2c_id);


static struct i2c_driver ma120x0_i2c_driver = {
	.driver = {
		.name = "ma120x0",
		.owner = THIS_MODULE,
		.of_match_table = ma120x0_of_match,
	},
	.probe = ma120x0_i2c_probe,
	.shutdown = ma120x0_i2c_shutdown,
	.id_table = ma120x0_i2c_id
};


static int __init ma120x0_modinit(void)
{
	int ret = 0;

	ret = i2c_add_driver(&ma120x0_i2c_driver);
	if (ret) {
		printk(KERN_ERR "Failed to register ma120x0 I2C driver: %d\n",
		       ret);
	}

	return ret;
}
module_init(ma120x0_modinit);


static void __exit ma120x0_exit(void)
{
	i2c_del_driver(&ma120x0_i2c_driver);
}
module_exit(ma120x0_exit);


MODULE_AUTHOR("Ariel Muszkat ariel.muszkat@infineon.com>");
MODULE_DESCRIPTION("ASoC driver for ma120x0");
MODULE_LICENSE("GPL v2");
