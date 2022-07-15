KERNEL = x.xx.xx-v7+
#Replace kernel with your kernel source version keep "-v7+" for Raspberry pi 3, "-v7l+" for Raspberry pi 4
#Current kernel version can be found with: $uname -r

REMOTE_RPI = pi@192.168.x.x
#Remote pi path - for remote installation.

REMOTE_PATH = /lib/modules/$(KERNEL)

LOCAL_BUILD = ~/Development/linux
#Replace this path with your local kernel source files.

CODECS_KCONFIG = $(LOCAL_BUILD)/sound/soc/codecs/Kconfig
CODECS_MAKEFILE = $(LOCAL_BUILD)/sound/soc/codecs/Makefile

BCM_KCONFIG = $(LOCAL_BUILD)/sound/soc/bcm/Kconfig
BCM_MAKEFILE = $(LOCAL_BUILD)/sound/soc/bcm/Makefile

ROOT_SD = rootfs

#Compile locally and install to external sd card. 
#(Useful for compiling on primary machine, and flashing to pi-sd card.)
all: clean pcodecs1 pcodecs2 pbcm1 pbcm2 poverlay crosscfg enablemodule crossmodules installmodules installbuild

#Compile locally and install externally with scp to a remote pi. 
#Works, but requires the host machine to have sudo access on the remote pi.
remote: pcodecs1 pcodecs2 poverlay crosscfg enablemodule crossmodules install

#Compile and install locally. (Typically done on the target pi.)
patch: pcodecs1 pcodecs2 pbcm1 pbcm2 poverlay enablemodule

pcodecs1:
	cp linux/sound/soc/codecs/ma120x0p.*  $(LOCAL_BUILD)/sound/soc/codecs ;\
	sed -i '/endmenu/d' $(CODECS_KCONFIG) ;\
	echo "config SND_SOC_MA120X0P" >> $(CODECS_KCONFIG) ;\
	echo "	tristate \"Merus Audio Multilevel Class D amplifier\"" >> $(CODECS_KCONFIG) ;\
	echo "	depends on I2C" >> $(CODECS_KCONFIG) ;\
	echo "" >> $(CODECS_KCONFIG) ;\
	echo "endmenu" >> $(CODECS_KCONFIG)

pcodecs2:
	echo "snd-soc-ma120x0p-objs := ma120x0p.o" >> $(CODECS_MAKEFILE) ;\
	echo "obj-\$$(CONFIG_SND_SOC_MA120X0P)	+= snd-soc-ma120x0p.o" >> $(CODECS_MAKEFILE)

pbcm1:
	cp linux/sound/soc/bcm/*.c $(LOCAL_BUILD)/sound/soc/bcm ;\
	echo "" >> $(BCM_KCONFIG) ;\
	echo "config SND_BCM2708_SOC_MERUS_AMP" >> $(BCM_KCONFIG) ;\
	echo "	tristate \"Support for Merus Audio Multilevel Amp \"" >> $(BCM_KCONFIG) ;\
	echo "	depends on SND_BCM2708_SOC_I2S || SND_BCM2835_SOC_I2S" >> $(BCM_KCONFIG) ;\
	echo "	select SND_SOC_MA120X0P" >> $(BCM_KCONFIG) ;\
	echo "	help" >> $(BCM_KCONFIG) ;\
	echo "	  Say Y or M if you want to add support for Merus Amp." >> $(BCM_KCONFIG)

pbcm2:
	echo "snd-soc-merus-amp-objs := merus-amp.o" >> $(BCM_MAKEFILE) ;\
	echo "obj-\$$(CONFIG_SND_BCM2708_SOC_MERUS_AMP) += snd-soc-merus-amp.o" >>  $(BCM_MAKEFILE)

poverlay:
	cp linux/arch/arm/boot/dts/overlays/*.dts $(LOCAL_BUILD)/arch/arm/boot/dts/overlays ;\
	sed -i '/justboom-digi.dtbo \\/ i\	merus-amp.dtbo \\' $(LOCAL_BUILD)/arch/arm/boot/dts/overlays/Makefile

crosscfg:
	cd $(LOCAL_BUILD) ;\
	KERNEL=kernel7l;\
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig

enablemodule:
	sed -i  '/# CONFIG_SND_BCM2708_SOC_MERUS_AMP is not set/ c\CONFIG_SND_BCM2708_SOC_MERUS_AMP=m' $(LOCAL_BUILD)/.config ;\
	sed -i '/# CONFIG_SND_SOC_MA120X0P is not set/ c\CONFIG_SND_SOC_MA120X0P=m' $(LOCAL_BUILD)/.config

crossoverlay:
		cd $(LOCAL_BUILD) ;\
		make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- dtbs -j 6

crossmodules:
	cd $(LOCAL_BUILD) ;\
	sudo rm -f arch/arm/boot/dts/overlays/merus-amp.dtbo
	cp linux/arch/arm/boot/dts/overlays/merus-amp-overlay.dts $(LOCAL_BUILD)/arch/arm/boot/dts/overlays ;\
	cp linux/sound/soc/codecs/ma120x0p.*  $(LOCAL_BUILD)/sound/soc/codecs ;\
	cp linux/sound/soc/bcm/merus-amp.c $(LOCAL_BUILD)/sound/soc/bcm ;\
	cd $(LOCAL_BUILD) ;\
	KERNEL=kernel7 ;\
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules dtbs zImage -j 6

crossimage:
	cd $(LOCAL_BUILD) ;\
	KERNEL=kernel7 ;\
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage -j 6

clean:
	sudo rm -f $(LOCAL_BUILD)/sound/soc/codecs/ma120x0p.*
	sudo rm -f $(LOCAL_BUILD)/sound/soc/codecs/.ma120x0p.*
	sudo rm -f $(LOCAL_BUILD)/sound/soc/codecs/snd-soc-ma120x0p.*
	sudo rm -f $(LOCAL_BUILD)/sound/soc/codecs/.snd-soc-ma120x0p.*
	sudo rm -f $(LOCAL_BUILD)/sound/soc/bcm/snd-soc-merus-amp.*
	sudo rm -f $(LOCAL_BUILD)/sound/soc/bcm/.snd-soc-merus-amp.*


clean_pi:
	sudo ssh -o StrictHostKeyChecking=no $(REMOTE_RPI) rm -f $(REMOTE_PATH)/kernel/sound/soc/codecs/snd-soc-ma120x0.ko ;\
	sudo ssh -o StrictHostKeyChecking=no $(REMOTE_RPI) rm -f $(REMOTE_PATH)/kernel/sound/soc/bcm/snd-soc-merus-amp.ko ;\
	sudo ssh -o StrictHostKeyChecking=no $(REMOTE_RPI) rm -f /boot/overlays/merus-amp.dtbo

install:
	sudo ssh -o StrictHostKeyChecking=no $(REMOTE_RPI) rm -f $(REMOTE_PATH)/kernel/sound/soc/codecs/snd-soc-ma120x0p.ko $(REMOTE_PATH)/kernel/sound/soc/bcm/snd-soc-merus-amp.ko /boot/overlays/merus-amp.dtbo ;\	
	sudo scp $(LOCAL_BUILD)/.config '$(REMOTE_RPI)://$(REMOTE_PATH)/kernel/sound/soc/codecs/' ;\
	sudo scp $(LOCAL_BUILD)/sound/soc/codecs/snd-soc-ma120x0p.ko '$(REMOTE_RPI)://$(REMOTE_PATH)/kernel/sound/soc/codecs/' ;\
	sudo scp $(LOCAL_BUILD)/sound/soc/bcm/snd-soc-merus-amp.ko '$(REMOTE_RPI)://$(REMOTE_PATH)/kernel/sound/soc/bcm/' ;\
	sudo scp $(LOCAL_BUILD)/arch/arm/boot/dts/overlays/merus-amp.dtbo '$(REMOTE_RPI):///boot/overlays/'

installmodules:
	sudo rm -R /media/$(USER)/$(ROOT_SD)/lib/modules/$(KERNEL) ;\
	cd $(LOCAL_BUILD) ;\
	sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=/media/$(USER)/$(ROOT_SD) modules_install ;\

installbuild:
	sudo rm -f /media/$(USER)/boot/kernel7.img ;\
	sudo rm -f /media/$(USER)/boot/*.dtb ;\
	sudo rm -f /media/$(USER)/boot/overlays/*.dtb*
	sudo cp $(LOCAL_BUILD)/arch/arm/boot/zImage /media/$(USER)/boot/kernel7.img ;\
	sudo cp $(LOCAL_BUILD)/arch/arm/boot/dts/*.dtb /media/$(USER)/boot/ ;\
	sudo cp $(LOCAL_BUILD)/arch/arm/boot/dts/overlays/*.dtb* /media/$(USER)/boot/overlays/
