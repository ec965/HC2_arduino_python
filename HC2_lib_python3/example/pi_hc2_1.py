#main HC2 raspberry pi interface script
#cross UART wires on HC2 and Raspberry Pi
#connect 3v3 to 3v3 and Gnd to Gnd
#Enoch Chau 2019
import serial
from HC2 import HC2_ROASCII

ser = serial.Serial(
	# port = '/dev/ttyACM0', #for arduino USB serial converter
	# port = '/dev/ttyUSB0', #for USB
	port = "/dev/serial0", #for Raspberry Pi 3B+ UART -> must disable bluetooth
		# disable bluetooth and enable serial
		#	1. add "enable_uart = 1" in /boot/config.txt
		# 	2. add "dtoverlay=pi3-miniuart-bt" in /boot/config.txt
		# 	3. sudo systemctl disable hciuart
		#	4. reboot
	baudrate = 19200,
	bytesize=serial.EIGHTBITS,
	parity=serial.PARITY_NONE,
	stopbits=serial.STOPBITS_ONE,
    xonxoff=False,
    rtscts=False,
    dsrdtr=False,
	timeout = 1,
	write_timeout = 2.5)


def print_cmd(rs485_addr):
	print("Options: ")
	print("0. RDD: Read humidity and temperature.")

	print("1. REN: Change the RS485 address of the device. Current address is: " + str(rs485_addr))

	print("2. HCA: Adjust calibration for humidity")
	print("3. HCA: Adjust calibration for temperature")
	print("4. HCA: Factory reset calibration for humidity")
	print("5. HCA: Factory reset calibration for temperature")

	print("6. LGC: See data recording status")
	print("7. LGC: Set data recording options, start/stop data recording, loop data recording")

	print("8. ERD: Download recorded data")
	print("9. exit program")

def cmd_input(cmd, probe):
	if cmd == 0 :
		probe.read_values()

	elif cmd == 1 :
		#rs485 address can be 0:63
		input_check = True
		while input_check:
			new_rs485_addr = input("What is the new rs485 address? please enter a number from 0 to 63: ")
			#weed out all addresses not in the range 0:63
			if int(new_rs485_addr) < 0 or int(new_rs485_addr) > 63:
				print("invalid option, please try again")
			else:
				input_check = False
		probe.change_rs485_addr(new_rs485_addr)

	elif cmd == 2:
		input_check = True
		while input_check:
			ref_val = input("What is the reference value to calibrate against?\n")
			ref_val = float(ref_val)
			if ref_val < 0 or ref_val > 100 :
				print("invalid option, please enter a number between 0 and 100")
			else:
				input_check = False
		probe.cal_hum_temp(1, ref_val)

	elif cmd == 3:
		input_check = True
		while input_check:
			ref_val = input("What is the reference value to calibrate against?\n")
			ref_val = float(ref_val)
			if ref_val < -100 or ref_val > 200 :
				print("invalid option, please enter a number between -100 and 200")
			else:
				input_check = False
		probe.cal_hum_temp(2, ref_val)

	elif cmd == 4:
		probe.factory_reset_cal(1)

	elif cmd == 5:
		probe.factory_reset_cal(2)

	elif cmd == 6:
		probe.data_rec_status()

	elif cmd == 7:
		print("The command to stop recording must be sent before a new recording can be started")
		print("Whenever the probe starts recording data, all previously recorded data is erased from the memory")
		probe.data_rec_status()

		start_stop = input("Input an option\n 0.Stop recording data\n 1.Start recording data\n")
		input_check = True
		while input_check:
			if int(start_stop) < 0 or int(start_stop) > 1 :
				print("invalid option, please try again")
			else:
				input_check = False

		startstop_or_loop = input("Input an option\n 1.Start/stop data recording\n 2.Loop data recording\n")
		input_check = True
		while input_check:
			if int(startstop_or_loop) < 1 or int(startstop_or_loop) > 2 :
				print("invalid option, please try again")
			else:
				input_check = False

		log_interval = input("What should be the log interval? 1 = 5s, 2 = 10s, 3 = 15s ...\n(note: floats will be casted to integers)\n")
		input_check = True
		while input_check:
			if int(log_interval) < 1 or int(log_interval) > 65535 :
				print("invalid option, please try again")
			else:
				input_check = False

		probe.record(start_stop, startstop_or_loop, log_interval)

	elif cmd == 8:
		output_filename = input("Please name the data export file (do not include file extension in export file name)\n")
		probe.download_data(output_filename)

	elif cmd == 9:
		print("exiting")
		return False
	else :
		print("invalid option, exiting")
		return False

	return True



if __name__ == "__main__":
	print('Serial port: ' + ser.name)
	probe = HC2_ROASCII(serial_port = ser)
	program_io = True

	while program_io == True:
		print_cmd(probe.rs485_addr)
		cmd = input("please enter a command: ")
		program_io = cmd_input(int(cmd), probe)
