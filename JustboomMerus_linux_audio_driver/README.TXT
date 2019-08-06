SVN check
Prep kernel for building

Follow normal get rpi kernel practice

copy target .config from /lib/modules/(Verion)/build/.config to kernel source code
scp root@rpi73.local:///lib/modules/4.14.90-1-ARCH/build/.config .

scripts/config --disable CC_STACKPROTECTOR_STRONG

Patch Makefile to make sure EXTRAVERSION in uname -a is correct



The Linux audio driver for MA120x0 device consist of driver code

Add to linux/sound/soc/bcm/Kconfig
Link between SND_BCM2708_SOC_JUSTBOOM_MERUS_AUDIO and SND_SOC_MA120X0

config SND_BCM2708_SOC_JUSTBOOM_MERUS_AUDIO
	tristate "Support for JustBoom Merus Audio Multilevel Amp"
	depends on SND_BCM2708_SOC_I2S || SND_BCM2835_SOC_I2S
	select SND_SOC_MA120X0
	help
	  Say Y or M if you want to add support for JustBoom Merus Audio.

Add to linux/sound/soc/bcm/Makefile
snd-soc-justboom-merus-audio-objs := justboom-merus-audio.o
obj-$(CONFIG_SND_BCM2708_SOC_JUSTBOOM_MERUS_AUDIO) += snd-soc-justboom-merus-audio.o

Add file
linux/sound/soc/bcm/justboom-merus-audio.c




linux/sound/soc/codec/ma120x0.c
  Register map abstration
  ALSA mixer configuration
  Low level rec/play configuration

Device overlay for rpi integration

linux/arch/arm/boot/dts/justboom-merus-audio-overlay.dts




Low level code : linux/sound/soc/codec/ma120x0.c

Add to Kconfig before endmenu at end of file

 config SND_SOC_MA120X0
	tristate "Merus Audio Multilevel Class D amplifier"
	depends on I2C

Add to Makefile before section obj-$(CONFIG...)

snd-soc-ma120x0-objs := ma120x0.o
	
And add end of file


obj-$(CONFIG_SND_SOC_MA120X0) += snd-soc-ma120x0.o
