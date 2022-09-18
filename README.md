# cy7c65213_gpio

The code here can be used to control pins on the CY7C65213 chip.

Building the code:
Download the SDK from the CY7C65213 web page, and in the examples folder, there is a gpio example. Replace the gpio.cpp file there with the one in this repository. Then, build the code using (say) Visual Studio.

Usage:

gpio.exe <output|input> <gpionumber> <0|1> <msec>

(Only output is currently supported)

