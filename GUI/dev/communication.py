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
from multiprocessing import *
import time
from tkinter import *
from tkinter.ttk import *

# Serial Ports & attributes.
INVALID_PORTS = []
VALID_PORTS = []
PORT_MC_LORA = None
PORT_CRAFT_LORA = None
PORT_CRAFT_MEGA = None
CONFIGURATION = None
MC_SHARED_LIST = None

PROCESS_MC_LORA = None
THREAD_CRAFT_LORA = None
THREAD_MEGA_LORA = None


def get_serial_ports():
	""" Detects and returns all active serial ports. """

	return list(serial.tools.list_ports.comports())

def setup_comms():
	""" 
	Oversees the serial port connection process.

	@param desired_connection - Developer specificed connection.
	"""

	ports = get_serial_ports()

	validate_ports(ports)

	start_threading()


def start_threading():
	""" Binds recieve methods to different processes to allow for simultaneous reads. """

	global PORT_MC_LORA
	global PROCESS_MC_LORA

	try:
		# Checks for active port.
		if PORT_MC_LORA is not None:
			with Manager() as manager:
				junk, port_num = int(PORT_MC_LORA.get_port().port.split(" "))
				print("Parsed com #: " + str(port_num))
				MC_SHARED_LIST = manager.list()
				MC_SHARED_LIST.append(port_num)
				PROCESS_MC_LORA = Process(target=receive_mc_lora, args=(MC_SHARED_LIST, ))
				PROCESS_MC_LORA.start()
				time.sleep(0.5)
		if PORT_CRAFT_LORA is not None:
			THREAD_CRAFT_LORA = threading.Timer(5.0, receive_craft_lora)
			THREAD_CRAFT_LORA.start()
		if PORT_CRAFT_MEGA is not None:
			THREAD_MEGA_LORA = threading.Timer(5.0, receive_craft_mega)
			THREAD_MEGA_LORA.start()
	except Exception as e:
		print("Unable to bind method to individual process.")
		print("Exception: " + str(e))
	

def validate_ports(ports):
	"""
	Pulls the connected microcontrollers for their name.

	@param ports - Detected serial port connections.
	"""

	global PORT_MC_LORA
	global PORT_CRAFT_MEGA
	global PORT_CRAFT_LORA

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

				time.sleep(1.5)
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


def generic_receive(ser):
	"""
	Responsible for reading in data on the given serial port.
	NON BLOCKING CALL.

	@param ser - Serial port instance.
	"""

	try:
		# Checks for a incoming data.
		if(ser.in_waiting != 0):
			# Reads in and decodes incoming serial data.
			message = ser.readline().decode()

			print("\nReceived: " + message)
			# Return data.
			return str(message)
		else:
			"No response."
	except:
		return "No response."


def receive_mc_lora(shared_list):
	"""
	Responsible for reading in data on the given serial port.
	THREAD LOCKED METHOD. DO NOT CALL.

	@param ser - Serial port instance.
	"""

	ser = serial.Serial()
	ser.port = "COM " + str(shared_list[0])
	ser.baudrate = 115200
	ser.timeout = 1

	if ser.is_open:
		ser.close()
	ser.open()

	while(1):
		try:
			# Checks for a incoming data.
			if(ser.in_waiting != 0):
				# Reads in and decodes incoming serial data.
				message = ser.readline().decode()
				# Sets data.
				shared_list[0] = str(message)
		except:
			time.sleep(0.5)


def receive_craft_lora(ser):
	"""
	Responsible for reading in data on the given serial port.
	THREAD LOCKED METHOD. DO NOT CALL.

	@param ser - Serial port instance.
	"""

	while(1):
		try:
			# Checks for a incoming data.
			if(ser.in_waiting != 0):
				# Reads in and decodes incoming serial data.
				message = ser.readline().decode()
				# Return data.
				return str(message)
		except:
			pass


def receive_craft_mega(ser):
	"""
	Responsible for reading in data on the given serial port.
	THREAD LOCKED METHOD. DO NOT CALL.

	@param ser - Serial port instance.
	"""

	while(1):
		try:
			# Checks for a incoming data.
			if(ser.in_waiting != 0):
				# Reads in and decodes incoming serial data.
				message = ser.readline().decode()
				# Return data.
				return str(message)
		except:
			pass


def send(ser, message):
	""" 
	Sends passed in parameter over the bound serial port.
	
	@param ser     - Developer specificed serial port.
	@param message - Paramter to be encoded and sent.
	"""

	# Ensure string datatype.
	message = str(message)

	print(message)

	if ser.is_open == False:
		ser.open()
		print("Opened port")

	# print("\nSending: " + message)
	# Encode message to bits & send via serial.
	ser.write(message.encode())


class serial_object():

	def __init__(self, ser, name, description):
		self.ser = ser
		self.port_name = name
		self.context = description
		self.input = StringVar()
		self.input.set("")

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
