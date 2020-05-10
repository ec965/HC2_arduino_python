#putting datetime into form acceptable by AirChip3000
#library for use with HC2.py

import datetime
import time
#use this version

#seconds from 1970 to 2000: 946684800

def encode_datetime() :
    now_time = time.time() - 946684800
    now_time = now_time/5
    return now_time

def decode_datetime(set_time) :
    set_time = set_time*5 + 946684800
    datetime_string = datetime.datetime.fromtimestamp(int(set_time))
    return str(datetime_string)

def decode_datetime_rtc(set_time) :
    set_time = set_time*5 + 946684800 + 8*60*60 #need to add 8 hrs for moteino time convert
    datetime_string = datetime.datetime.fromtimestamp(int(set_time))
    return str(datetime_string)
