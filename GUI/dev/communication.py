#############################################################
#
#	Property of Eagle Eye. 
#
#   Authors:
#           Jared Danner
#
#############################################################
import serial, serial.tools.list_ports
import threading
import time

# Serial Ports & attributes.
INVALID_PORTS = []
VALID_PORTS = []
PORT_MC_LORA = None
PORT_CRAFT_LORA = None
PORT_CRAFT_MEGA = None
CONFIGURATION = None

THREAD_MC_LORA = None
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
	""" Binds recieve methods to different threads to allow for simultaneous reads. """

	try:
		# Checks for active port.
		if PORT_MC_LORA is not None:
			THREAD_MC_LORA = threading.Thread(target=receive_mc_lora, args=(PORT_MC_LORA.get_port()))
			THREAD_MC_LORA.start()
		if PORT_CRAFT_LORA is not None:
			THREAD_CRAFT_LORA = threading.Thread(target=receive_craft_lora, args=(PORT_CRAFT_LORA.get_port()))
			THREAD_CRAFT_LORA.start()
		if PORT_CRAFT_MEGA is not None:
			THREAD_MEGA_LORA = threading.Thread(target=receive_craft_mega, args=(PORT_CRAFT_MEGA.get_port()))
			THREAD_MEGA_LORA.start()
	except:
		print("Unable to bind method to thread.")
	
def validate_ports(ports):
	"""
	Pulls the connected microcontrollers for their name.

	@param ports - Detected serial port connections.
	"""

	# COM number.
	com_number = ""
	# Micro-controller descriptor.
	port_description = ""
	# Temporary serial port variable.
	ser = None

	# Cycles over all detected ports.
	for port in ports:

		passed = True
		# Parses out port info.
		try:
			com_number, port_description = str(port).split("-")
			com_number = com_number.strip()
			port_description = port_description.strip()
		except:
			print("Unable to parse: " + str(port))
			passed = False
			ser.close()

		# Attempts to recreate serial connection to ensure validity.
		try: 
			if passed:
				ser = serial.Serial()
				ser.port = com_number
				ser.baudrate = 115200
				ser.open()
		except (serial.SerialException):
			print("Invalid connection to: " + str(port))
			print("Port: " + str(ser))
			passed = False
			ser.close()

		# Pings microcontroller for name.
		try:
			if passed:
				send(ser, "PING")
				response = generic_receive(ser)

				# System blocking pause for 1 second.
				time.sleep(1)

				if response in "CRAFT_LORA":
					PORT_CRAFT_LORA = serial_object(ser, response, port_description)
				elif response in "CRAFT_MEGA":
					PORT_CRAFT_LORA = serial_object(ser, response, port_description)
				elif response in "MC_LORA":
					PORT_CRAFT_LORA = serial_object(ser, response, port_description)
		except:
			print("Unknown microcontroller. Response: " + str(response))


def generic_receive(ser):
	"""
	Responsible for reading in data on the given serial port.
	NON BLOCKING CALL. USE THIS RECE
	@param ser - Serial port instance.
	"""

	try:
		# Checks for a incoming data.
		if(ser.in_waiting != 0):
			# Reads in and decodes incoming serial data.
			message = ser.readline().decode()
			# Return data.
			return str(message)
	except:
		return "No response."


def receive_mc_lora(ser):
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

	if ser.is_open == False:
		ser.open()

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
