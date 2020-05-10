//Enoch Chau 2019
#ifndef HC2_h
#define HC2_h

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "ac3k_datetime.h"
#include "RTClib.h"

class HC2{
public:
  HC2(SoftwareSerial* serial_port);
  //INIT
  String write_rdd(char write_id, String write_rs485);
  bool init();
  //tool
  bool is_ok(String output_string);
  //RDD
  void read_values();
  //REN
  bool change_rs485_addr(String new_rs485_addr);
  //HCA
  bool cal_hum_temp(uint8_t hum_or_temp, int16_t ref_val);
  bool factory_reset_cal(uint8_t hum_or_temp);
  //LGC
  void data_rec_status();
  void record(bool stop_start, uint8_t startstop_or_loop, uint16_t log_interval, uint32_t set_time);
  //ERD
  void download_data(String output_filename, uint32_t end_time);

  //Functions that return raw data stream from HC2
  //RDD
  String read_values_ds();
  void decode_rdd_ds(String output);
  //LGC
  String data_rec_status_ds();
  void decode_lgc_ds(String output);



private:
  char _id;
  String _rs485_addr;
  String _serial_num;
  SoftwareSerial* _serial_port;
};

#endif
