MERUS(TM) Linux Audio Driver for MA120x0 multi-level Class D amplifiers for Volumio

To build the Linux audio Driver into a Volumio distribution:

- You will need a working Volumio distribution running on Kernel 4.14.92+ (you can burn Volumiomerusamp.img from link) and a Linux machine (e.g. Ubuntu)
- Turn on the Raspberry pi, configure the wifi network and enable ssh. (details on volumio.org)
- Enable root login in the Raspberry pi by editing the file /etc/ssh/sshd_config, change PermitRootLogin to "yes"
- Add a root password in the Raspberry pi: type sudo passwd root and reboot the pi
- git clone merus_linux_audio_driver_volumio folder
- Add desired patches/changes to the source code:
  ma120x0.c
  merus-amp.c
  merus-amp-overlay.dts
- git clone the kernel source from link and save it into your home folder
- git clone raspberry pi toolchain: git clone https://github.com/raspberrypi/tools ~/tools
-update the $PATCH environment variable
  if in a 64bit-machine:
  type: echo PATH=\$PATH:~/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin >> ~/.bashrc
source ~/.bashrc
  if in a 32bit machine:
  echo PATH=\$PATH:~/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin >> ~/.bashrc
source ~/.bashrc
 - to compile the source type: make crossmodules
 - to clean any old version of the software on Volumio kernel source type: make clean
 - to install the new compiled modules type: make install
 - 
  ```

  ```
