# MERUS™ Audio Amp HAT ZW Linux Drivers - Source code

<img src="https://github.com/Infineon/merus-audio-amp-hat-zw/blob/master/KIT_40W_AMP_HAT_ZW_webpage.png" style="max-width:100%;" width="400">


Linux ALSA SoC driver of Infineon's [MERUS™ Audio Amp HAT ZW](https://www.infineon.com/cms/en/product/evaluation-boards/tle94112el_shield/) for Raspberry pi Zero W.

## Summary
The MERUS™ Audio Amp is a Hardware at the Top (HAT) audio power amplifier board which brings the Infineon proprietary multi-level technology to Raspberry pi users and makers. It is intended for loudspeaker building and wireless music streaming with minimum size and consumption, state of the art power efficiency and HD audio quality. The MERUSTM Audio Amp is equipped with the MA12070P Class D multi-level amplifier which can provide up to 40W instantaneous peak power from the official Raspberry pi supply and up to 48KHz/24bit music playback. Furthermore, there is no need for extra power supplies, the MERUS™ Audio Amp is powered directly from the same supply the Raspberry pi Zero W is running from. It is compatible with the most popular audio streaming applications for Raspberry pi such as Volumio, Max2Play, Justboom player, and it can be also used in Raspbian as an Airplay client or Bluetooth audio receiver.

## Key Features and Benefits
-	Equipped with MERUSTM MA12070P [MERUS™ MA12070P](https://www.infineon.com/cms/en/product/power/class-d-audio-amplifier-solutions/integrated-class-d-audio-amplifier-ics/ma12070p/).
 proprietary multi-level amplifier.
-	Compatible with Raspberry pi Zero and raspberry pi Zero Wireless
-	Compatibility with major streaming applications for Raspberry pi (Volumio, Justboom Player, Max2play)
-	Power input: 5V/2.5A (sourced from the same single supply as the Raspberry pi)
-	No need for external or extra power supplies
-	Up to 40W instantaneous peak output power with the Raspberry pi official 5V/2.5A supply
-	Up to 48KHz of sample rate and 24 bit of music playback.
-	THD+N: 0.077% @ 7W/4ohm/1Khz
-	Full Hardware control, customization, and error monitoring trough linux alsamixer.
-	Built-in boost converter from 5V to 20V to supply the MA12070P device.
-	I2S digital audio input
-	I2C communication for full register map control
-	Automatic enable control for booster and amplifier for secure boot-up and shutdown sequences.
-	Digital limiter for loudspeaker protection.
-	2xBTL Channel (Bridge Tied Loaded) default configuration
-	Optional 1xPBTL (Parallel Bridge Tied Loaded) configuration for higher power True wireless Stereo applications

## Target Applications:
* Wireless speakers
* Hifi audio systems
* Smart Speakers
* Multiroom audio

## Installation
For detailed description on how to install, set-up and use the MERUS™ Audio Amp HAT ZW board please follow the [quick start guide](https://www.infineon.com/dgdl/Infineon-Quickstartguide_KIT_40W_AMP_HAT_ZW%20-AdditionalTechnicalInformation-v01_00-EN.pdf?fileId=5546d4626eab8fbf016eeb75f58a6326) or the [user's manual](https://www.infineon.com/dgdl/Infineon-KIT_40W_AMP_HAT_ZW-UserManual-v01_00-EN.pdf?fileId=5546d4626eab8fbf016eef8084096be6).

Pre-compiled images for Volumio and Raspbian are available with built-in MERUS™ HAT drivers. In order to install an image download the desired file from the [releases section](https://github.com/Infineon/merus-audio-amp-hat-zw/releases) of the repository and flash it with [balena etcher](https://www.balena.io/etcher/).

Additionally (for advanced users working on linux host machines), different installation scripts and procedures of the MA120x0P linux audio driver are covered in this repository. This is specially useful if the board and/or any MA120x0P device needs to be used with other Raspberry pi versions and other linux distributions. Also, it allows to easily modify, compile and build the driver according to the target application if this is also a requirement:
- merus_linux_audio_driver: Raspbian distribution on Raspberry pi Zero W
- merus_linux_audio_driver_pi3:Raspbian distribution on Raspberry pi 3
- merus_linux_audio_driver_volumio: Volumio distribution on Raspberry pi zero W
- merus_linux_audio_driver_volumio: Volumio distribution on Raspberry pi 3

It is recommended to follow the [Kernel and Driver building app note](https://www.infineon.com/dgdl/Infineon-KIT_40W_AMP_HAT_ZW-ApplicationNotes-v01_00-EN.pdf?fileId=5546d4626eab8fbf016eef808ad46be9) if the previous installation scripts are going to be used.

## Board Information, Datasheet and Additional Information
* [User Manual](https://www.infineon.com/dgdl/Infineon-KIT_40W_AMP_HAT_ZW-UserManual-v01_00-EN.pdf?fileId=5546d4626eab8fbf016eef8084096be6).
* [MA12070P Datasheet](https://www.infineon.com/dgdl/Infineon-MA12070P-DS-v01_00-EN.pdf?fileId=5546d46264a8de7e0164b761f2f261e4)
* [Kernel and Drivers Building app note](https://www.infineon.com/dgdl/Infineon-KIT_40W_AMP_HAT_ZW-ApplicationNotes-v01_00-EN.pdf?fileId=5546d4626eab8fbf016eef808ad46be9).
