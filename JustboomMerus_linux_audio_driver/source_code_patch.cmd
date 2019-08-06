

# Add
justboom-merus-audio.dtbo \
# after justboom-digi.dtbo \
to file inux/arch/arm/boot/dts/overlays/Makefile

=======================================================================================

# Add justboom-merus-audio.c driver to sound/soc/bcm/

# In sound/soc/bcm/Makefile
# add line
obj-$(CONFIG_SND_BCM2708_SOC_JUSTBOOM_MERUS_AUDIO) += snd-soc-justboom-merus-audio.o
# after obj-$(CONFIG_SND_BCM2708_SOC_JUSTBOOM_DIGI) += snd-soc-justboom-digi.o
# add line
snd-soc-justboom-merus-audio-objs := justboom-merus-audio.o
# after snd-soc-justboom-digi-objs := justboom-digi.o


# in sound/soc/bcm/Kconfig
# add

config SND_BCM2708_SOC_JUSTBOOM_MERUS_AUDIO
	tristate "Support for JustBoom Merus Audio Multilevel Amp"
	depends on SND_BCM2708_SOC_I2S || SND_BCM2835_SOC_I2S
	select SND_SOC_MA120X0
	help
	  Say Y or M if you want to add support for JustBoom Merus Audio.

After other JUSTBOOM sections

====================================================================================================

Enable new drivers in .config find and replace

CONFIG_SND_BCM2708_SOC_JUSTBOOM_MERUS_AUDIO=m
CONFIG_SND_SOC_MA120X0=m

====================================================================================================
# copy modules to target
cp sound/soc/codecs/snd-soc-codec-ma120x0.ko /lib/modules/<kernel_version>/kernel/sound/soc/codecs/
cp sound/soc/bcm/snd-soc-justboom-merus-audio.ko /lib/moduels/<kernel_version>/kernel/sound/soc/bcm/

# run depmon on target
cd /lib/modules/<kernelversion>/
depmod
