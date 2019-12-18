# MERUS™ Audio Amp HAT ZW Linux Drivers

<img src="https://github.com/Infineon/merus-audio-amp-hat-zw/blob/master/KIT_40W_AMP_HAT_ZW_webpage.jpg" style="max-width:100%;" width="400">


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
For the description on how to use the MERUS™ Audio Amp HAT ZW board please follow the [quick start guide](docs/Installation.md) or the [user's manual](docs/Installation.md). Additionally (for advanced users working on linux host machines), different installation scripts and procedures of the MA120x0P linux audio driver are covered in this repository. This is specially useful if the board and/or any MA120x0P device needs to be used with other Raspberry pi versions and other linux distributions. Also, it allows to easily modify, compile and build the driver according to the target application if this is also a requirement:
- merus_linux_audio_driver: Raspbian distribution on Raspberry pi Zero W
- merus_linux_audio_driver_pi3:Raspbian distribution on Raspberry pi 3
- merus_linux_audio_driver_volumio: Volumio distribution on Raspberry pi zero W



## Examples
For the description of the [examples](docs/Examples.md)

## Library documentation
The doxygen [library documentation](https://infineon.github.io/DC-Motor-Control-TLE94112EL/).


## Board Information, Datasheet and Additional Information
* A PDF summarizing the features and layout of the DC motor control shield is stored on the Infineon homepage [here](https://www.infineon.com/dgdl/Infineon-DC_Motor_Control_Shield_with_TLE94112EL_UserManual-UM-v01_00-EN.pdf?fileId=5546d46259d9a4bf015a4755351304ac).
* The datasheet for the TLE94112EL can be found here [TLE94112EL Datasheet](https://www.infineon.com/dgdl/Infineon-TLE94112EL-DS-v01_00-EN.pdf?fileId=5546d462576f347501579a2795837d3e)
* The respective application note is located here [TLE941xy Application Note](https://www.infineon.com/dgdl/Infineon-TLE941xy-AN-v01_00-EN-AN-v01_00-EN-AN-v01_00-EN.pdf?fileId=5546d4625b62cd8a015bc8db26c831e3).
* There is a pinout picture [Tle94112el pintout](https://raw.githubusercontent.com/infineon/assets/master/Pictures/TLE94112_Arduino_Shield_Pin_out.png)
