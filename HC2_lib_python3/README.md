# Raspberry Pi Python library
The script will automatically capture the RS485 address of the HC2 sensor.

# Arduino as USB to serial converter
An Arduino UNO can be used as a USB to serial converter. If you choose to use this method use the following instructions:

Tx    -> Tx
Rx    -> Rx
3v3   -> 3v3
GND   -> GND
RESET -> GND
(it's funky but don't cross tx and rx)

plug into USB port
on raspberry pi: use /dev/ttyACM0
