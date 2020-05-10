#Enoch Chau 2019
#main library for HC2 serial commands for use with Raspberry Pi
import time
import serial
import ac3k_datetime
import math
import csv

class HC2_ROASCII:
    def __init__(self, serial_port):
        self.serial_port = serial_port
        self.id = self.get_id()
        self.rs485_addr = self.get_rs485_addr()
        self.serial_num = self.get_serial_num()


    #Request RDD to get init values
    def write_rdd(self,id,rs485_addr):
        # print("reading values")
        pencil = "{" + str(id) + "99RDD}\r\n"
        # print(pencil)
        self.serial_port.write(pencil.encode())
        output = str(self.serial_port.readline())

        # print(output)

        return output

    #initilization functions
    def get_rs485_addr(self):
        output = self.write_rdd(self.id,99)
        print("rs485 address: " + output[4:6])
        return output[4:6]

    def get_serial_num(self):
        output = self.write_rdd(self.id,self.rs485_addr)
        print("serial number: " + output[83:93])
        return output[83:93]

    def get_id(self):
        alphabet = ['A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z']
        for id_check in alphabet:
            output = self.write_rdd(str(id_check),99)
            # print(id_check)
            if len(output) > 2:
                break
        print("ID: " + output[3])
        return output[3]

	# check for OK
    def is_ok(self,output_string):
        if output_string[10:12] == 'OK':
            return True
        else:
            return False

	#RDD
    def read_values(self):
        print("reading values")
        pencil = "{" + str(self.id) + str(self.rs485_addr) + "RDD}\r\n"
        self.serial_port.write(pencil.encode())
        output = str(self.serial_port.readline())

        print(output)

        split_output = output.split(';')
        # for i in range(0,len(split_output)):
        #     print(str(i) + ": " + split_output[i])
        print("ID_Address_Cmd: " + split_output[0][3:9])
        print("Humidity: " + split_output[1] + "%")
        print("Temperature: " + split_output[5] + " 'C")
        print("Firmware Version: " + split_output[15])
        print("Serial Number: " + split_output[16])
        print("Model: " + split_output[17])

	#REN
    #rs485 address can be 0:63
    def change_rs485_addr(self, new_rs485_addr):
        print("changing RS485 address")
        pencil = "{" + str(self.id) + str(self.rs485_addr) + "REN " + str(self.serial_num) + ";" + str(new_rs485_addr) + ";}\r\n"
        self.serial_port.write(pencil.encode())
        output = str(self.serial_port.readline())

        print(output)

        if self.is_ok(output):
            print("address change successful!")
            self.rs485_addr = new_rs485_addr
        else:
            print("address failed to change!")

	#HCA
	#calibrate humidity or temperature, hum_or_temp = 1 to change humididty, hum_or_temp = 2 to change temperature
	#ref_val is the referance value for calibration
    def cal_hum_temp(self, hum_or_temp, ref_val):
        timeout_save = self.serial_port.timeout
        self.serial_port.timeout = 3
        print("saving adjustment")
        pencil = "{" + str(self.id) + str(self.rs485_addr) + "HCA 0;" + str(hum_or_temp) + ";0;" + str(ref_val) + ";}\r\n"
        self.serial_port.write(pencil.encode())
        output = str(self.serial_port.readline())

        print(output)

        if self.is_ok(output) :
            print("adjusting")
            pencil = "{" + str(self.id) + str(self.rs485_addr) + "HCA 0;" + str(hum_or_temp) + ";1;;}\r\n"
            self.serial_port.write(pencil.encode())
            output = str(self.serial_port.readline())

            print(output)

            if self.is_ok(output) :
                print("deleting calibration points")
                pencil = "{"  + str(self.id) + str(self.rs485_addr) + "HCA 0;" + str(hum_or_temp) + ";3;;}\r\n"
                self.serial_port.write(pencil.encode())
                output = str(self.serial_port.readline())

                print(output)

                if self.is_ok(output) :
                    print("calibration is complete")
                else :
                    return print("calibration failed to delete calibration points")
            else :
                return print("calibration failed to adjust")
        else :
            return print("calibration failed to save adjustment")
        self.serial_port.timeout = timeout_save

	# hum_or_temp = 1 to change humididty, hum_or_temp = 2 to change temperature
    def factory_reset_cal(self, hum_or_temp) :
        timeout_save = self.serial_port.timeout
        self.serial_port.timeout = 3
        print("reseting to factory default calibration")
        pencil = "{"  + str(self.id) + str(self.rs485_addr) + "HCA 0;" + str(hum_or_temp) + ";2;;}\r\n"
        self.serial_port.write(pencil.encode())
        output = str(self.serial_port.readline())

        print(output)

        if self.is_ok(output) :
            return print("factory reset calibration successful!")
        else :
            return print("factory reset calibration failed!")
        self.serial_port.timeout = timeout_save

	#LGC
	#show status of data recording
    def data_rec_status(self) :
        print("showing data recording status")
        pencil = "{"  + str(self.id) + str(self.rs485_addr) + "LGC}\r\n"
        self.serial_port.write(pencil.encode())
        output = str(self.serial_port.readline())

        print(output)
		#finish this when you know what the output string will look like
        split_output = output.split(';')
        # for i in range(0,len(split_output)):
        #     print(str(i) + ": " + split_output[i])

        #Recording status
        # 0 = not recording data, 1= recording data
        # Loop recording only: 2 = recording data and memory full (see note below)
        # Loop recording only: 3 = not recording data and memory full (see note below)
        recording_status = split_output[0][11:13]
        if int(recording_status) == 0 :
            recording_status = "not recording data"
        elif int(recording_status) == 1 :
            recording_status = "recording data"
        elif int(recording_status) == 2 :
            recording_status = "recording data and memory full"
        elif int(recording_status) == 3:
            recording_status = "not recording data and memory full"

        #Recording Mode
        # 1 = Start-Stop mode (record until memory is full)
        # 2 = Loop recording (dump oldest data when memory is full and keep recording)
        recording_mode = split_output[1]
        if int(recording_mode) == 1 :
            recording_mode = "Start-Stop mode (record until memory is full)"
        elif int(recording_mode) == 2 :
            recording_mode = "Loop recording (dump oldest data when memory is full and keep recording)"

        #Log Interval
        # Log interval (in multiples of 5 seconds) 1 = 5s, 2 = 10s, etc.
        log_interval = int(split_output[2])
        log_interval = log_interval*5

        #Date & time of first data sample
        # Date and time of the first data sample referenced to Jan, 01, 2000 and expressed in
        # increments of 5 seconds – see note 2 below
        start_time = "date & time : " + ac3k_datetime.decode_datetime(int(split_output[3]))


        #Number of data records
        data_amount = split_output[4]

        print("ID_Address_Cmd: " + split_output[0][3:9])
        print("recording data?: " + recording_status)
        print("recording mode: " + recording_mode)
        print("data logging interval: " + str(log_interval) + " seconds")
        print(start_time)
        print("Amount of data points recorded: " + str(data_amount))


	#stop or start recording data
	#to start/stop recording: startstop_or_loop = 1
	#to loop recording: startstop_or_loop = 2
	#log_interval in values of 5s. ex: 1 = 5s, 2 = 10s...
    def record(self, stop_start, startstop_or_loop, log_interval) :
        print("setting data recording options")
		#set_time is in 5 second intervals since start of 1 Jan. 2000

        set_time = ac3k_datetime.encode_datetime()
        pencil = "{"  + str(self.id) + str(self.rs485_addr) + "LGC " + str(stop_start) + ";" + str(startstop_or_loop) + ";" + str(log_interval) + ";" + str(int(set_time)) + ";" + "}\r\n"

        print(pencil)

        self.serial_port.write(pencil.encode())
        output = str(self.serial_port.readline())

        print(output)

        if self.is_ok(output) :
            return print("data recording options updated")
        else :
            return print("data recording options failed to update, please check options")

	#ERD
	#bytes_to_read can be from 0 to 65535
	#each data sample uses 3 bytes
    def download_data(self, output_filename) :
        print("downloading data")
        #Date & time of last data sample
        end_time = time.localtime(time.time())

		# 1) Use the LGC command to read the recording status of the HC2. Note or capture the following:
		# - Recording mode
		# - Log interval (multiple of 5 seconds)
		# - Recording start date and time (see note below)
		# - Number of recorded data samples (in the loop recording mode, disregard the information returned by the
		# LGC command whenever the LGC command reports that the memory is full. In this situation, use 2000
		# as the number of data samples)
        pencil = "{"  + str(self.id) + str(self.rs485_addr) + "LGC}\r\n"
        self.serial_port.write(pencil.encode())
        output = str(self.serial_port.readline())

        print(output)

        split_output = output.split(';')

        #Recording status
        recording_status = split_output[0][11:13]
        if int(recording_status) == 0 :
            recording_status = "not recording data"
        elif int(recording_status) == 1 :
            recording_status = "recording data"
        elif int(recording_status) == 2 :
            recording_status = "recording data and memory full"
        elif int(recording_status) == 3:
            recording_status = "not recording data and memory full"
        #Recording Mode
        recording_mode = split_output[1]
        if int(recording_mode) == 1 :
            recording_mode = "Start-Stop mode (record until memory is full)"
        elif int(recording_mode) == 2 :
            recording_mode = "Loop recording (dump oldest data when memory is full and keep recording)"
        #Log Interval
        raw_log_interval = int(split_output[2])
        log_interval = raw_log_interval*5
        #Date & time of first data sample
        start_time_encoded = int(split_output[3])
        start_time = ac3k_datetime.decode_datetime(int(split_output[3]))
        #Number of data records
        data_amount = split_output[4]
        data_amount = int(data_amount)
        #max memory is 2000 values
        if data_amount > 2000:
            data_amount = 2000



		#2)
		# Address 2176 is always the address of the first byte of the first recorded data sample. The above example
		# assumes that 2 data samples were recorded. The number of bytes to be downloaded (0006) is equal to 3
        #  times the number of recorded data samples (each data sample uses 3 Bytes)


        #bytes_to_read should be four digits like: 0006
        #bytes_to_read should be assigend from LGC
        bytes_to_read = str(int(data_amount)*3).zfill(4)
        pencil = "{"  + str(self.id) + str(self.rs485_addr) + "ERD 0;2176;" + str(bytes_to_read) + "}\r\n"
        print(pencil)
        self.serial_port.write(pencil.encode())
        output = str(self.serial_port.readline())

        print(output)

		#iterate through a split up output string and perform calculations to get the data
		#output this data to a file or server or something else

        output = output[10:]
        split_output = output.split(';')
        del split_output[-1]

        hum = []
        temp = []

        # for i in range(len(split_output)):
        #     print(split_output[i])
        #     print(i)
        # print(len(split_output))
        for i in range(0, len(split_output), 3) :
            # print(i)
            # print(str(split_output[i]) + ";" + str(split_output[i+1]) + ";" + str(split_output[i+2]))
            value = int(split_output[i]) + 256*int(split_output[i+1]) + 65536*int(split_output[i+2])
            value = float(value)
            hum.append( (value%1024)/10 )
            temp.append(round(math.floor(value/1024) /20-100, 2))

        #saving data to txt file
        # output_file = open('/home/pi/Desktop/'+str(output_filename)+".txt", "w")
        # output_file.write("date & time (format: dd/mm/yyyy hh:mm:ss)\n")
        # output_file.write("Start time:" + start_time+"\n")
        # output_file.write("note: start time should only be used for Start-Stop mode or if loop mode is not at max data collected\n")
        # end_time_string = "date & time (format: dd/mm/yyyy hh:mm:ss) : " + str(end_time.tm_mday) + "/" + str(end_time.tm_mon) + "/" + str(end_time.tm_year) + " " + str(end_time.tm_hour) + ":" + str(end_time.tm_min) + ":" + str(end_time.tm_sec)
        # output_file.write("\nEnd time: " + end_time_string+"\n")
        # output_file.write("note: end time should only be used if loop mode is at max data collected. End time is the time that ERD is called")
        # output_file.write("\nData:\n")
        # for i in range(len(hum)) :
        #     output_file.write(str(i)+". "+str(hum[i])+"%rh    "+str(temp[i])+"'C\n")
        # output_file.close()

        #saving file as CSV
        #time-stamp only works for start-stop mode currently
        with open('/home/pi/Desktop/'+str(output_filename)+".csv", mode ="w") as csv_file:
            fieldnames = ['Time (dd/mm/yyyy hh:mm:ss)','Humidity (%rh)','Temperature (\'C)']
            writer = csv.DictWriter(csv_file, fieldnames=fieldnames)

            writer.writeheader()
            for i in range(len(hum)) :
                data_time_encoded = start_time_encoded + i*raw_log_interval
                data_time = ac3k_datetime.decode_datetime(int(data_time_encoded))
                writer.writerow({'Time (dd/mm/yyyy hh:mm:ss)': data_time,'Humidity (%rh)': str(hum[i]),'Temperature (\'C)':str(temp[i])})

        print("Your data has been saved to " + str(output_filename) + '.csv')
