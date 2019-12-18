# Build script for merus-amp sound card with MA120x0P  multi-level Class D amplifiers codec driver

## for Volumio and Raspberry pi Zero W.

To build and install the merus-amp sound card and MA120x0P linux audio driver into a Volumio distribution with Raspberry pi zero w:

- You will need a working Volumio distribution running on Kernel 4.14.92+ (you can burn Volumiomerusamp.img) and a Linux machine (e.g. Ubuntu)
- Turn on the Raspberry pi, configure the wifi network and enable ssh. (details on volumio.org)
- Enable root login in the Raspberry pi by editing the file /etc/ssh/sshd_config, change PermitRootLogin to "yes"
- Add a root password in the Raspberry pi: type `sudo passwd root` and reboot the pi
- `git clone https://github.com/Infineon/merus-audio-amp-hat-zw.git`
- cd to merus_linux_audio_driver_volumio folder
- Add desired patches/changes to the source code:
  ma120x0.c
  merus-amp.c
  merus-amp-overlay.dts
- git clone the [raspberry pi kernel 4.14.92+ source](https://github.com/raspberrypi/linux/tree/24737fa6bcf1d7ffb71ceb78d7a7c275cb7e1d13)  (or desired) into your home folder
- git clone raspberry pi toolchain: `git clone https://github.com/raspberrypi/tools ~/tools`
- update the $PATCH environment variable
  if in a 64bit-machine:
  type: `echo PATH=\$PATH:~/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin >> ~/.bashrc
  source ~/.bashrc`
  if in a 32bit machine:
  `echo PATH=\$PATH:~/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin >> ~/.bashrc
  source ~/.bashrc`
 - Copy .config into your kernel source downloaded previously (If you are using another kernle version you need to use it's .config file)
 - to compile the source type: `make crossmodules`
 - to clean any old version of the software (if you installed the drivers before) on Volumio kernel source type: `make clean`
 - to install the new compiled modules type: `make install`
 - login into the Raspberry pi with Volumio and go to /lib64/modules/14.14.92+. Then type `sudo modprobe`
 - Make sure to replace the config.txt file line "dtparamaudio=on" with "dtoverlay=merus-amp" to load the driver during boot up.
 - Finally reset the Raspberry pi.
