# Build script for merus-amp sound card with MA120x0P  multi-level Class D amplifiers codec driver

## for Raspbian and Raspberry pi Zero W.

To build and install the merus-amp sound card and MA120x0P linux audio driver into a Raspbian distribution with a Raspberry pi zero w.

# Detailed process
- Follow the process covered on the [app note](https://www.infineon.com/dgdl/Infineon-KIT_40W_AMP_HAT_ZW-ApplicationNotes-v01_00-EN.pdf?fileId=5546d4626eab8fbf016eef808ad46be9)

# Steps sum-up

- You will need a working Raspbian distribution (you can burn [Raspbianmerusamp.img from this link](link) ) and a Linux host machine (e.g. Ubuntu)
- `git clone https://github.com/Infineon/merus-audio-amp-hat-zw.git`
- cd to merus_linux_audio_driver folder
- Add desired patches/changes to the source code:
  ma120x0.c
  merus-amp.c
  merus-amp-overlay.dts
- git clone the [raspberry pi kernel source](https://github.com/raspberrypi/linux) into your home folder
- git clone raspberry pi toolchain: `git clone https://github.com/raspberrypi/tools ~/tools``
- update the $PATCH environment variable
  if in a 64bit-machine type:
  `echo PATH=\$PATH:~/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin >> ~/.bashrc
  source ~/.bashrc`
  if in a 32bit machine:
  `echo PATH=\$PATH:~/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin >> ~/.bashrc
  source ~/.bashrc`
 - Insert the SD card on your linux host machine
 - to compile, build and install the drivers type: `make all`
 - Make sure to replace the config.txt file line "dtparamaudio=on" with "dtoverlay=merus-amp" to load the driver during boot up.
 - Insert the SD card on the Raspberry pi.
 - Finally reset the Raspberry pi.
