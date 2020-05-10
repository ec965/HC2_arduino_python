# Rotronics HC2

This is a library to interface a Raspberry Pi or Arduino UNO with the Rotronics HC2.

The HC2 is a serial device and uses Rotronics proprietary ASCII format called ROASCII. It requires an RTC to initialize the time of the chip. More information can be found in the datasheets.

HC2 Functions:
1. RDD: read values
2. REN: change RS485 address
3. HCA
  1. change Humidity
  2. factory reset humidity
  3. change Temperature
  4. factory reset Temperature
4. LGC
  1. read status of data recording function
  2. program data recording function
    1. stop or start recording and options
    4. probe counts seconds since 1 Jan. 2000
5. ERD: download recorded data
6. TID: set date and time
