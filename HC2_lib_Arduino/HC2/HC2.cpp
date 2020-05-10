//Enoch Chau 2019
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "HC2.h"
#include "ac3k_datetime.h"
#include "RTClib.h"

//assign serial port & rtc
HC2::HC2(SoftwareSerial * serial_port){
  _serial_port = serial_port;
}

//used to init
String HC2::write_rdd(char write_id, String write_rs485 ){
  _serial_port->print("{");
  _serial_port->print(write_id);
  _serial_port->print(write_rs485);
  _serial_port->println("RDD}");

  String output = _serial_port->readString();

  return output;
}

//initlization of id, rs485 address, and serial number
bool HC2::init(){
  bool init_pass = false;

  String output = write_rdd('F',"99");

  if (output != ""){
    init_pass=true;
  }
  // Serial.println(output);
  _id = output.charAt(1);
  _rs485_addr = output.substring(2,4);
  _serial_num = output.substring(75,85);

  Serial.print("ID: "); Serial.println(_id);
  Serial.print("RS485_address: "); Serial.println(_rs485_addr);
  Serial.print("Serial Number: "); Serial.println(_serial_num);
  return init_pass;
}

bool HC2::is_ok(String output_string){
  bool check = false;
  Serial.println(output_string.substring(8,10));
  if (output_string.substring(8,10) == "OK"){
    check = true;
  }
  return check;
}

//read temp and humidity
//RDD
void HC2::read_values(){
  // Serial.println("reading values...");
  String output = write_rdd(_id,_rs485_addr);

  // Serial.println(output);

  String hum = output.substring(13,18);
  String temp = output.substring(30,35);
  String fware_version = output.substring(68,74);
  String model = output.substring(86,89);
  Serial.print("ID: "); Serial.println(_id);
  Serial.print("RS485_address: "); Serial.println(_rs485_addr);
  Serial.print("Serial Number: "); Serial.println(_serial_num);
  Serial.print("Humidity: "); Serial.print(hum);Serial.println("%rh");
  Serial.print("Temperature: "); Serial.print(temp);Serial.println("'C");
  Serial.print("Firmware Version: "); Serial.println(fware_version);
  Serial.print("Model: "); Serial.println(model);
}

//REN
//change RS485 address, addresses can be 0:63
//address must be a two digit string, example: "01", "06", "62"
bool HC2::change_rs485_addr(String new_rs485_addr){
  bool ok = false;

  _serial_port->print("{");
  _serial_port->print(_id);
  _serial_port->print(_rs485_addr);
  _serial_port->print("REN ");
  _serial_port->print(_serial_num);
  _serial_port->print(";");
  _serial_port->print(new_rs485_addr);
  _serial_port->println(";}");

  String output = _serial_port->readString();

  // Serial.println(output);
  ok = is_ok(output);
  if (ok){
    _rs485_addr = new_rs485_addr;
  }
  return(ok);
}

// HCA
// #calibrate humidity or temperature, hum_or_temp = 1 to change humididty, hum_or_temp = 2 to change temperature
// #ref_val is the referance value for calibration
//must ask user for a refereance value for adjustment
//ref value can be from -50 to 200
bool HC2::cal_hum_temp(uint8_t hum_or_temp, int16_t ref_val){
  bool ok = false;
  Serial.println("Saving Adjustment...");

  _serial_port->print("{");
  _serial_port->print(_id);
  _serial_port->print(_rs485_addr);
  _serial_port->print("HCA 0;");
  _serial_port->print(hum_or_temp);
  _serial_port->print(";0;");
  _serial_port->print(ref_val);
  _serial_port->println(";}");

  String output = _serial_port->readString();

  Serial.println(output);

  if (is_ok(output)){
    delay(20);
    Serial.println("Adjusting...");

    _serial_port->print("{");
    _serial_port->print(_id);
    _serial_port->print(_rs485_addr);
    _serial_port->print("HCA 0;");
    _serial_port->print(hum_or_temp);
    _serial_port->println(";1;;}");

    delay(30);
    output = _serial_port->readString();

    Serial.println(output);

    if (is_ok(output)){
      delay(20);
      Serial.println("Deleting Calibration Points...");

      _serial_port->print("{");
      _serial_port->print(_id);
      _serial_port->print(_rs485_addr);
      _serial_port->print("HCA 0;");
      _serial_port->print(hum_or_temp);
      _serial_port->println(";3;;}");

      output = _serial_port->readString();

      Serial.println(output);

      if (is_ok(output)){
        delay(20);
        // Serial.println("Calibration succesful");
        ok = true;
        return(ok);
      }

      else{
        Serial.println("Calibration failed to delete calibration points");
        return(ok);
      }
    }
    else{
      Serial.println("Calibration failed to adjust");
      return(ok);
    }
  }
  else{
    Serial.println("Calibration failed to save adjustment");
    return(ok);
  }
}

//	# hum_or_temp = 1 to change humididty, hum_or_temp = 2 to change temperature
bool HC2::factory_reset_cal(uint8_t hum_or_temp){
  bool ok = false;
  // Serial.println("Resetting to factory default calibration...");

  _serial_port->print("{");
  _serial_port->print(_id);
  _serial_port->print(_rs485_addr);
  _serial_port->print("HCA 0;");
  _serial_port->print(hum_or_temp);
  _serial_port->println(";2;;}");

  String output = _serial_port->readString();

  // Serial.println(output);

  delay(20);
  ok = is_ok(output);
  return(ok);
}

//LGC
//show status of data recording
void HC2::data_rec_status(){
  Serial.println("showing data recording status.");

  _serial_port->print("{");
  _serial_port->print(_id);
  _serial_port->print(_rs485_addr);
  _serial_port->println("LGC}");

  String output = _serial_port->readString();

  Serial.println(output);

  // #Recording status
  //     # 0 = not recording data, 1= recording data
  //     # Loop recording only: 2 = recording data and memory full (see note below)
  //     # Loop recording only: 3 = not recording data and memory full (see note below)
  String recording_status = "";
  if (output[10] == '0'){
    recording_status = "not recording data";
  }
  else if (output[10] == '1'){
    recording_status = "recording data";
  }
  else if (output[10] == '2'){
    recording_status = "recording data and memory full";
  }
  else if (output[10] == '3'){
    recording_status = "not recording data and memory full";
  }

  // #Recording Mode
  //     # 1 = Start-Stop mode (record until memory is full)
  //     # 2 = Loop recording (dump oldest data when memory is full and keep recording)
  String recording_mode = "";
  if (output[14] == '1'){
    recording_mode = "Start-Stop mode (record until memory is full)";
  }
  else if (output[14] == '2'){
    recording_mode = "Loop recording (dump oldest data when memory is full and keep recording)";
  }

  // #Log Interval
  // # Log interval (in multiples of 5 seconds) 1 = 5s, 2 = 10s, etc.

  // String log_interval_str = output.substring(16,21);
  // int log_interval_int = log_interval_str.toInt();
  // log_interval_int = log_interval_int*5;
  int log_interval_int = output.substring(16,21).toInt();
  log_interval_int = log_interval_int*5;


  // #Date & time of first data sample
  // # Date and time of the first data sample referenced to Jan, 01, 2000 and expressed in
  // # increments of 5 seconds – see note 2 below
  DateTime start_time = decode_datetime(output.substring(22,32).toInt());

  Serial.print("start time: ");
  Serial.println(start_time.timestamp());

  Serial.print("recording data?: ");
  Serial.println(recording_status);

  Serial.print("recording mode: ");
  Serial.println(recording_mode);

  Serial.print("data logging interval: ");
  Serial.print(log_interval_int);
  Serial.println(" sec.");

  Serial.print("Amount of data points recorded: ");
  Serial.println(output.substring(33,38));
}

// #stop or start recording data
// #to start/stop recording mode: startstop_or_loop = 1
// #to loop recording mode: startstop_or_loop = 2
// #log_interval in values of 5s. ex: 1 = 5s, 2 = 10s...
void HC2::record(bool stop_start, uint8_t startstop_or_loop, uint16_t log_interval, uint32_t set_time){
  Serial.println("setting data recording options");

  _serial_port->print("{");
  _serial_port->print(_id);
  _serial_port->print(_rs485_addr);
  _serial_port->print("LGC ");
  _serial_port->print(stop_start);
  _serial_port->print(";");
  _serial_port->print(startstop_or_loop);
  _serial_port->print(";");
  _serial_port->print(log_interval);
  _serial_port->print(";");
  _serial_port->print(set_time);
  _serial_port->println(";}");

  String output = _serial_port->readString();

  Serial.println(output);

  if (is_ok(output)){
    Serial.println("Data recording options updated!");
  }
  else{
    Serial.println("Data recording options failed to update!");
  }
}

// #ERD
// #bytes_to_read can be from 0 to 65535
// #each data sample uses 3 bytes
void HC2::download_data(String output_filename, uint32_t end_time){
  Serial.println("Downloading Data");

  _serial_port->print("{");
  _serial_port->print(_id);
  _serial_port->print(_rs485_addr);
  _serial_port->println("LGC}");

  Serial.println("LGC done");

  String output = _serial_port->readString();

  Serial.println(output);

//start time
  DateTime start_time = decode_datetime(output.substring(22,32).toInt());

  //log interval, raw log interval in increments of 5sec
  int log_interval_int = output.substring(16,21).toInt();

//Number of data records; max memory is 2000 values
// #bytes_to_read should be four digits like: 0006
// #bytes_to_read should be assigend from LGC
  int bytes_to_read = output.substring(33,38).toInt() * 3;

  Serial.println("starting erd");

  _serial_port->print("{");
  _serial_port->print(_id);
  _serial_port->print(_rs485_addr);
  _serial_port->print("ERD 0;2176;");
  if (bytes_to_read < 1000){ //zero padding
    _serial_port->print(0);
  };
  if (bytes_to_read < 100){
    _serial_port->print(0);
  };
  if (bytes_to_read < 10){
    _serial_port->print(0);
  };
  _serial_port->print(bytes_to_read);
  _serial_port->println("}");

  Serial.println("erd done");
  delay(3000);
  //each byte_to_read is 4 chars// ex: '038;'
  //the header and checksum are 9 chars
  output = "";
  while(_serial_port->available()>0){
    output += (char)_serial_port->read();
    delay(10);
  }


  // output = _serial_port->readString();
  //
  Serial.println(output);

  //remove the header
  //do calculations to get hum and temp
  //export that data
}

//--------------------------------------------------------------
//Functions that return raw data stream from HC2

//read temp and humidity
//RDD

String HC2::read_values_ds(){
  // Serial.println("reading values...");
  String output = write_rdd(_id,_rs485_addr);
  // Serial.println(output);
  return(output);
}

void HC2::decode_rdd_ds(String output){
  String hum = output.substring(13,18);
  String temp = output.substring(30,35);
  String fware_version = output.substring(68,74);
  String model = output.substring(86,89);
  Serial.print("ID: "); Serial.println(_id);
  Serial.print("RS485_address: "); Serial.println(_rs485_addr);
  Serial.print("Serial Number: "); Serial.println(_serial_num);
  Serial.print("Humidity: "); Serial.print(hum);Serial.println("%rh");
  Serial.print("Temperature: "); Serial.print(temp);Serial.println("'C");
  Serial.print("Firmware Version: "); Serial.println(fware_version);
  Serial.print("Model: "); Serial.println(model);
}
//LGC
String HC2::data_rec_status_ds(){

  _serial_port->print("{");
  _serial_port->print(_id);
  _serial_port->print(_rs485_addr);
  _serial_port->println("LGC}");

  String output = _serial_port->readString();

  Serial.println(output);
  return(output);
}

void decode_lgc_ds(String output){
  String recording_status = "";
  if (output[10] == '0'){
    recording_status = "not recording data";
  }
  else if (output[10] == '1'){
    recording_status = "recording data";
  }
  else if (output[10] == '2'){
    recording_status = "recording data and memory full";
  }
  else if (output[10] == '3'){
    recording_status = "not recording data and memory full";
  }
  // #Recording Mode
  //     # 1 = Start-Stop mode (record until memory is full)
  //     # 2 = Loop recording (dump oldest data when memory is full and keep recording)
  String recording_mode = "";
  if (output[14] == '1'){
    recording_mode = "Start-Stop mode (record until memory is full)";
  }
  else if (output[14] == '2'){
    recording_mode = "Loop recording (dump oldest data when memory is full and keep recording)";
  }

  // #Log Interval
  // # Log interval (in multiples of 5 seconds) 1 = 5s, 2 = 10s, etc.

  // String log_interval_str = output.substring(16,21);
  // int log_interval_int = log_interval_str.toInt();
  // log_interval_int = log_interval_int*5;
  int log_interval_int = output.substring(16,21).toInt();
  log_interval_int = log_interval_int*5;


  // #Date & time of first data sample
  // # Date and time of the first data sample referenced to Jan, 01, 2000 and expressed in
  // # increments of 5 seconds – see note 2 below
  DateTime start_time = decode_datetime(output.substring(22,32).toInt());

  Serial.print("start time: ");
  Serial.println(start_time.timestamp());

  Serial.print("recording data?: ");
  Serial.println(recording_status);

  Serial.print("recording mode: ");
  Serial.println(recording_mode);

  Serial.print("data logging interval: ");
  Serial.print(log_interval_int);
  Serial.println(" sec.");

  Serial.print("Amount of data points recorded: ");
  Serial.println(output.substring(33,38));
}
