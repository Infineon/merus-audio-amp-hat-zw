# Build script for merus-amp sound card with MA120x0P  multi-level Class D amplifiers codec driver

## for Raspbian and Raspberry pi 3.

To build and install the merus-amp sound card and MA120x0P linux audio driver into a Raspbian distribution with a Raspberry pi 3.

# Detailed process
- Follow the process covered on the [app note](https://www.infineon.com/dgdl/Infineon-KIT_40W_AMP_HAT_ZW-ApplicationNotes-v01_00-EN.pdf?fileId=5546d4626eab8fbf016eef808ad46be9)

# Steps sum-up
1 - You will need a working Raspbian distribution (you can burn [Raspbianmerusamp.img from this link](link) ) and a Linux host machine (e.g. Ubuntu).

2 - `git clone https://github.com/Infineon/merus-audio-amp-hat-zw.git`

3 - cd to merus_linux_audio_driver_pi3 folder

4 - (Optionally and if desired), add patches/changes to the source code:
  ma120x0.c
  merus-amp.c
  merus-amp-overlay.dts

5 - git clone the [raspberry pi kernel source](https://github.com/raspberrypi/linux) into your home folder

6 - Install neccesary packaged and headers: `sudo apt-get install git bison flex libssl-dev gcc-arm-linux-gnueabihf make`

7 - git clone raspberry pi toolchain: `git clone https://github.com/raspberrypi/tools ~/tools`

8 - update the $PATCH environment variable

  if in a 64bit machine type:
  `echo PATH=\$PATH:~/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin >> ~/.bashrc
  source ~/.bashrc`

  if in a 32bit machine:
  `echo PATH=\$PATH:~/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin >> ~/.bashrc
  source ~/.bashrc`

9 - Insert the SD card on your linux host machine

10 - Replace the "KERNEL = xx.xx.xx-v7+" variable on the first line of the Makefile in merus_linux_audio_driver_pi3  with your downloaded kernel source
    version on step 5. to check the downloaded kernel source open the Makefile in the cloned linux folder in of step 5 and check for the following lines:

    "VERSION = 4
    PATCHLEVEL = 19
    SUBLEVEL = 97"

    (in this case this would result in "KERNEL = 4.19.97-v7+")

11 - to compile, build and install the drivers type: `make all` .

12 - Make sure to replace the config.txt file line "dtparam=audio=on" with "dtoverlay=merus-amp" to load the driver during boot up.

13 - Insert the SD card on the Raspberry pi.

14  - Finally reboot the Raspberry pi.
