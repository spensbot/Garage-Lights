# Garage Lights

The repository is distributed under the GNU GPLv3 license.

Welcome to the Garage Lights project!

This project aims to create open source audio plugins (in multiple formats) that allow simple control of DMX lighting within a DAW. The plugin will provide a DMX universe (512 channels) of parameters that users can control within the DAW via automation, midi control, the plugin GUI, or any other method the DAW provides for parameter adjustment.

Created with the Juce application framework. The contents of this repository can be copied into a JUCE projucer project "Source" folder to be compiled. https://juce.com/get-juce

Currently only supports ENNTEC DMX Pro compatible USB interfaces. Several of the functions in "UsbDmxPro.h" are based on a C++ code example found on ENNTEC's site. https://www.enttec.com/us/products/controls/dmx-usb/open-dmx-usb/

Communicates with the USB device using the included FTDI driver. The included driver is for windows 64 bit. Additional drivers can be downloaded here: http://www.ftdichip.com/Drivers/D2XX.htm


I'm not opposed to adding features/fixing bugs within the code. Let me know if you have ideas/problems and I might look into it :)
This is currently only compiled in VST2 and VST3 64bit formats. If you need another format let me know. I might try to help.

I created this plugin because I searched far and wide for something with the same features.  It either doesn't exist, or I couldn't find it... Until now. 
Hope you enjoy the plugin!
