#############################################################
#
#	Property of Eagle Eye. 
#
#   Authors:
#           Jared Danner
#
#############################################################
import serial, serial.tools.list_ports

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
				response = receive(ser)

				if response in "CRAFT_LORA":
					PORT_CRAFT_LORA = serial_object(ser, response, port_description)
				elif response in "CRAFT_MEGA":
					PORT_CRAFT_LORA = serial_object(ser, response, port_description)
				elif response in "MC_LORA":
					PORT_CRAFT_LORA = serial_object(ser, response, port_description)


def receive(ser):
	"""
	Responsible for reading in data on the given serial port.

	@param ser - Serial port instance.
	"""

	# Checks for a incoming data.
    if(ser.in_waiting != 0):
    	# Reads in and decodes incoming serial data.
        message = ser.readline().decode()
        # Return data.
		return str(message)
	# Returns empty.
	return ""


def send(ser, message):
	""" 
	Sends passed in parameter over the bound serial port.
	
	@param ser     - Developer specificed serial port.
	@param message - Paramter to be encoded and sent.
	"""

	# Ensure string datatype.
	message = str(message)
	# Encode message to bits & send via serial.
	ser.write(message.encode())

class serial_object():

	def __init__(self, ser, name, description):
		self.ser = ser
		self.port_name = name
		self.context = description

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


