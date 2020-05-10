//Enoch Chau 2019
//use RTC_DS3231

//i2c address is 0x68
#include "RTClib.h"
#include <Arduino.h>
#include <Wire.h>

uint32_t encode_datetime(RTC_DS3231 * rtc){
  DateTime now = rtc->now();
  uint32_t sec_time = now.secondstime();
  uint32_t sec5time = sec_time/5;

  // Serial.println(sec5time);

  return sec5time;
}

DateTime decode_datetime(uint32_t sec5time){
  uint32_t unix_time = SECONDS_FROM_1970_TO_2000 + (sec5time * 5);
  DateTime decoded_time (unix_time);
  return decoded_time;
}
