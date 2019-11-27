KERNEL = 4.14.92+

REMOTE_RPI = root@172.20.10.5

REMOTE_PATH = /lib64/modules/$(KERNEL)

LOCAL_BUILD = /home/$(USER)/linux

CODECS_KCONFIG = $(LOCAL_BUILD)/sound/soc/codecs/Kconfig
CODECS_MAKEFILE = $(LOCAL_BUILD)/sound/soc/codecs/Makefile

BCM_KCONFIG = $(LOCAL_BUILD)/sound/soc/bcm/Kconfig
BCM_MAKEFILE = $(LOCAL_BUILD)/sound/soc/bcm/Makefile

ROOT_SD = rootfs

all: clean crossmodules install

patch: pcodecs1 pcodecs2 pbcm1 pbcm2

pcodecs1 :
	cp linux/sound/soc/codecs/ma120x0.*  $(LOCAL_BUILD)/sound/soc/codecs ;\
	sed -i '/endmenu/d' $(CODECS_KCONFIG) ;\
	echo "config SND_SOC_MA120X0" >> $(CODECS_KCONFIG) ;\
	echo "	tristate \"Merus Audio Multilevel Class D amplifier\"" >> $(CODECS_KCONFIG) ;\
	echo "	depends on I2C" >> $(CODECS_KCONFIG) ;\
	echo "" >> $(CODECS_KCONFIG) ;\
	echo "endmenu" >> $(CODECS_KCONFIG)

pcodecs2 :
	echo "snd-soc-ma120x0-objs := ma120x0.o" >> $(CODECS_MAKEFILE) ;\
	echo "obj-\$$(CONFIG_SND_SOC_MA120X0)	+= snd-soc-ma120x0.o" >> $(CODECS_MAKEFILE)

pbcm1 :
	cp linux/sound/soc/bcm/*.c $(LOCAL_BUILD)/sound/soc/bcm ;\
	echo "" >> $(BCM_KCONFIG) ;\
	echo "config SND_BCM2708_SOC_MERUS_AMP" >> $(BCM_KCONFIG) ;\
	echo "	tristate \"Support for Merus Audio Multilevel Amp \"" >> $(BCM_KCONFIG) ;\
	echo "	depends on SND_BCM2708_SOC_I2S || SND_BCM2835_SOC_I2S" >> $(BCM_KCONFIG) ;\
	echo "	select SND_SOC_MA120X0" >> $(BCM_KCONFIG) ;\
	echo "	help" >> $(BCM_KCONFIG) ;\
	echo "	  Say Y or M if you want to add support for Merus Amp." >> $(BCM_KCONFIG)

pbcm2 :
	echo "snd-soc-merus-amp-objs := merus-amp.o" >> $(BCM_MAKEFILE) ;\
	echo "obj-\$$(CONFIG_SND_BCM2708_SOC_MERUS_AMP) += snd-soc-merus-amp.o" >>  $(BCM_MAKEFILE)

poverlay :
	cp linux/arch/arm/boot/dts/overlays/*.dts $(LOCAL_BUILD)/arch/arm/boot/dts/overlays ;\
	sed -i '/justboom-digi.dtbo \\/ i\	merus-amp.dtbo \\' $(LOCAL_BUILD)/arch/arm/boot/dts/overlays/Makefile

crosscfg :
	cd /home/$(USER)/linux ;\
	KERNEL=kernel ;\
	time make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2835_defconfig

enablemodule :
	sed -i '1 a CONFIG_SND_BCM2708_SOC_MERUS_AMP=m' $(LOCAL_BUILD)/.config ;\
	sed -i '1 a CONFIG_SND_SOC_MA120X0=m' $(LOCAL_BUILD)/.config ;\

crossoverlay :
		cd /home/$(USER)/linux ;\
		time make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- dtbs -j 6

crossmodules :
	sudo rm -R $(LOCAL_BUILD)/sound/soc/codecs/ma120x0.* ;\
	sudo rm -R $(LOCAL_BUILD)/sound/soc/bcm/merus-amp.* ;\
	sudo rm -R $(LOCAL_BUILD)/sound/soc/codecs/snd-soc-ma120x0.* ;\
	sudo rm -R $(LOCAL_BUILD)/sound/soc/bcm/snd-soc-merus-amp.* ;\
	cp linux/sound/soc/codecs/ma120x0.*  $(LOCAL_BUILD)/sound/soc/codecs ;\
	cp linux/sound/soc/bcm/merus-amp.c $(LOCAL_BUILD)/sound/soc/bcm ;\
	cd /home/$(USER)/linux ;\
	KERNEL=kernel ;\
	time make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules dtbs zImage -j 6

crossimage :
	cd /home/$(USER)/linux ;\
	KERNEL=kernel ;\
	time make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage -j 6

clean:
	sudo ssh -o StrictHostKeyChecking=no $(REMOTE_RPI) rm -f $(REMOTE_PATH)/kernel/sound/soc/codecs/snd-soc-ma120x0.ko ;\
	sudo ssh -o StrictHostKeyChecking=no $(REMOTE_RPI) rm -f $(REMOTE_PATH)/kernel/sound/soc/bcm/snd-soc-merus-amp.ko ;\
	sudo ssh -o StrictHostKeyChecking=no $(REMOTE_RPI) rm -f /boot/overlays/merus-amp.dtbo

install:
	sudo scp /home/$(USER)/linux/sound/soc/codecs/snd-soc-ma120x0.ko '$(REMOTE_RPI)://$(REMOTE_PATH)/kernel/sound/soc/codecs/' ;\
	sudo scp /home/$(USER)/linux/sound/soc/bcm/snd-soc-merus-amp.ko '$(REMOTE_RPI)://$(REMOTE_PATH)/kernel/sound/soc/bcm/' ;\
	sudo scp linux/arch/arm/boot/dts/overlays/merus-amp.dtbo '$(REMOTE_RPI)://boot/overlays/'

installmodules:
	sudo rm -R /media/$(USER)/$(ROOT_SD)/lib/modules/$(KERNEL) ;\
	cd /home/$(USER)/linux ;\
	sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=/media/$(USER)/$(ROOT_SD) modules_install ;\

installbuild:
	sudo rm -R /media/$(USER)/boot/kernel.img ;\
	sudo rm -R /media/$(USER)/boot/*.dtb ;\
	sudo rm -R /media/$(USER)/boot/overlays/*.dtb*
	sudo cp /home/$(USER)/linux/arch/arm/boot/zImage /media/$(USER)/boot/kernel.img ;\
	sudo cp /home/$(USER)/linux/arch/arm/boot/dts/*.dtb /media/$(USER)/boot/ ;\
	sudo cp /home/$(USER)/linux/arch/arm/boot/dts/overlays/*.dtb* /media/$(USER)/boot/overlays/
