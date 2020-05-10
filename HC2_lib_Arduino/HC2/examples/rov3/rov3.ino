//Enoch Chau 2019
//RTC ds32231 i2c address: 0x68
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "HC2.h"
#include "ac3k_datetime.h"
#include "RTClib.h"

SoftwareSerial ser(4,5); // RX, TX
RTC_DS3231 rtc;
HC2 probe (&ser);


// RTC_DS3231 rtc;

void setup() {
 Serial.begin(9600);
 ser.begin(19200);

 while (!Serial) ; // Wait for serial port to be available

 Serial.println("<Arduino is ready>");

 // init RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  //init HC2
 Serial.println("Initilizing...");
 if(!probe.init()){
   Serial.println("HC2 init failed");
   while (1);
 }


}

void loop() {
  //RDD test
  probe.read_values();

  //REN test
  // Serial.println("changing RS485 address to: 01");
  // if(probe.change_rs485_addr("01")){
  //   Serial.println("address change successful");
  // }
  // else{
  //   Serial.println("address change failed");
  // }
  // delay(20);
  // Serial.println("changing RS485 address to: 15");
  // if(probe.change_rs485_addr("15")){
  //   Serial.println("address change successful");
  // }
  // else{
  //   Serial.println("address change failed");
  // }
  // delay(20);

  //HCA test
  // Serial.println("Humidity factory reset");
  // if (probe.factory_reset_cal(1)){
  //   Serial.println("Factory reset calibration successful!");
  // }
  // else {
  //   Serial.println("Factory reset calibration failed!");
  // }
  //
  // Serial.println("Calibrating Temp");
  // if(probe.cal_hum_temp(2,14)){
  //   Serial.println("Calibration succesful");
  // }
  //
  // Serial.println("Temp factory reset");
  // if (probe.factory_reset_cal(2)){
  //   Serial.println("Factory reset calibration successful!");
  // }
  // else {
  //   Serial.println("Factory reset calibration failed!");
  // }
  //
  // //Humidity calibration is bugged, doesn't get past "Adjusting...""
  // Serial.println("Calibrating Humidity");
  // if(probe.cal_hum_temp(1,14)){
  //   Serial.println("Calibration succesful");
  // }
  // delay(100);

  //LGC read test
  // probe.data_rec_status();
  // delay(20);
  //LGC write test
  // probe.record(0, 1, 2, encode_datetime(&rtc));
  // delay(20);
  //ERD test
  // probe.download_data("test", encode_datetime(&rtc));
  // delay(20);

}
