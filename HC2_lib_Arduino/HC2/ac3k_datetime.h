//Enoch Chau 2019

#ifndef AC3K_DATETIME_h
#define AC3K_DATETIME_h

//use RTC_DS3231
#include "RTClib.h"
#include <Arduino.h>
#include <Wire.h>

uint32_t encode_datetime(RTC_DS3231 * rtc);

DateTime decode_datetime(uint32_t sec5time);

#endif
