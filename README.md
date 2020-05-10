# Rotronics HC2

This is a library to interface a Raspberry Pi or Arduino UNO with the Rotronics HC2.

The HC2 is a serial device and uses Rotronics proprietary ASCII format called ROASCII. It requires an RTC to initialize the time of the chip. More information can be found in the datasheets.

HC2 Functions:
1. RDD: read values
2. REN: change RS485 address
3. HCA
   * change Humidity
   * factory reset humidity
   * change Temperature
   * factory reset Temperature
4. LGC
   * read status of data recording function
   * program data recording function
     * stop or start recording and options
     * probe counts seconds since 1 Jan. 2000
5. ERD: download recorded data
6. TID: set date and time
