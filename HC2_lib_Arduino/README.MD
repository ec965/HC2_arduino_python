# Arduino Library for Rotronics HC2

This library was tested on Arduino UNO.

Functions that rely on time require a real time clock module. This library uses the DS3231 but any module can be used. Some modification to ac3k_datetime.cpp and ac3k_datetime.h may be nessesary if you are using a different module.

Please use the RTClib library: https://github.com/adafruit/RTClib

The HC2 is a serial device so for Arduino UNO, SoftwareSerial must be used to interface with the sensor. For other boards that have multiple serial ports, modifications to the code may be needed.
