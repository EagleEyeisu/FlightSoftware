#############################################################
#
#	Property of Eagle Eye. 
#
#   Authors:
#           Jared Danner
#
#############################################################
import serial.tools.list_ports
import serial
import time
from globals import *
from tkinter import *
from tkinter.ttk import *


def setup_comms():
	""" 
	Oversees the serial port connection process.

	@param desired_connection - Developer specificed connection.
	"""

	ports = get_serial_ports()

	validate_ports(ports)

	config_scheduler()
	

def get_serial_ports():
	""" Detects and returns all active serial ports. """

	return list(serial.tools.list_ports.comports())


def validate_ports(ports):
	"""
	Pulls the connected microcontrollers for their name.

	@param ports - Detected serial port connections.
	"""

	# COM number.
	com_number = ""
	# Micro-controller descriptor.
	port_description = ""

	# Cycles over all detected ports.
	for port in ports:

		# Temporary serial port variable.
		ser = None

		passed = True
		# Parses out port info.
		try:
			com_number, port_description = str(port).split("-")
			com_number = com_number.strip()
			port_description = port_description.strip()
		except Exception as e:
			passed = False
			print("Exception: " + str(e))
			print("Unable to parse: " + str(port))

		# Attempts to recreate serial connection to ensure validity.
		try: 
			if passed:
				ser = serial.Serial()
				ser.port = com_number
				ser.baudrate = 115200
				ser.timeout = 1
				ser.open()
		except Exception as e:
			passed = False
			print("Exception: " + str(e))
			print("Invalid connection to: " + str(port))

		# Pings microcontroller for name.
		try:
			if passed:
				send(ser, "PING")

				time.sleep(0.1)
				response = generic_receive(ser)

				if response in "CRAFT_LORA":
					PORT_CRAFT_LORA = serial_object(ser, response, port_description)
				elif response in "CRAFT_MEGA":
					PORT_CRAFT_MEGA = serial_object(ser, response, port_description)
				elif response in "MC_LORA":
					PORT_MC_LORA = serial_object(ser, response, port_description)
				else:
					print("Unknown Micro-controller: " + str(response))
		except Exception as e:
			passed = False
			print("Exception: " + str(e))
			print("Unknown Response: " + str(response))

		# Prints all info related to port (used for debug in case of failure).
		if not passed:
			print("\nport:" + str(port))
			print("com_number: " + com_number)
			print("port_description: " + port_description)
			print("ser object: " + str(ser))
			print("ser.port: " + ser.port)
			print("ser.baudrate: " + str(ser.baudrate))
			print("port status: " + str(ser.is_open) + "\n")


def config_scheduler():
	"""
	Schedules a timer like object to run a method to capture 
	serial input every x seconds. 
	"""

	try:
		# Starts scheduler.
		sched.start()
		# Checks which ports are active.
		if PORT_MC_LORA is not None:
			sched.add_job(mc_lora_receive,
						 'interval', 
						 id='mc_read', 
						 seconds=1)
		if PORT_CRAFT_LORA is not None:
			sched.add_job(craft_lora_receive,
						 'interval', 
						 id='craft_lora_read', 
						 seconds=1)
		if PORT_CRAFT_MEGA is not None:
			sched.add_job(craft_mega_receive,
						 'interval', 
						 id='craft_mega_read', 
						 seconds=1)
	except Exception as e:
		print("Unable to bind method to individual process.")
		print("Exception: " + str(e))


def generic_receive(ser):
	"""
	Responsible for reading in data on the given serial port.
	THIS METHOD RETURNS.

	@param ser - Serial port instance.
	"""

	try:
		# Checks for a incoming data.
		if(ser.in_waiting != 0):
			# Reads in and decodes incoming serial data.
			message = ser.readline().decode()

			print("Received from " + str(ser.port) + ". Input: " + message + "\n")
			
			# Return data.
			return str(message)
		else:
			return "No response."
	except Exception as e:
		print("Exception: " + str(e))
		return "No response."


def mc_lora_receive():
	"""
	Responsible for reading in data on the given serial port.

	@param ser - Serial port instance.
	"""

	ser = PORT_MC_LORA.get_port()

	try:
		# Checks for a incoming data.
		if(ser.in_waiting != 0):
			# Reads in and decodes incoming serial data.
			message = ser.readline().decode()

			print("Received from " + str(ser.port) + ". Input: " + message + "\n")
			
			# Return data.
			PORT_MC_LORA.set_input(str(message))
	except Exception as e:
		print("Exception: " + str(e))
		PORT_MC_LORA.set_input("Serial Error")


def send(ser, message):
	""" 
	Sends passed in parameter over the bound serial port.
	
	@param ser     - Developer specificed serial port.
	@param message - Paramter to be encoded and sent.
	"""

	# Ensure string datatype.
	message = str(message)

	print("\nSent to " + str(ser.port) + ". Message: " + message)

	if ser.is_open == False:
		ser.open()

	# print("\nSending: " + message)
	# Encode message to bits & send via serial.
	ser.write(message.encode())


class serial_object():

	def __init__(self, ser, name, description):
		self.ser = ser
		self.port_name = name
		self.context = description
		self.input = ""

	def get_context(self):
		"""
		Returns context info.

		@param self - Instance of the class.
		"""

		return self.context

	def get_port_name(self):
		"""
		Returns port ID.

		@param self - Instance of the class.
		"""

		return self.port_name

	def get_port(self):
		"""
		Returns port object.

		@param self - Instance of the class.
		"""

		return self.ser

	def set_input(self, new_input):
		"""
		Sets class input.

		@param self      - Instance of the class.
		@param new_input - Brand new serial port data.
		"""

		self.input = new_input
