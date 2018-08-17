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

def setup_comms(desired_connection):
	""" 
	Oversees the serial port connection process.

	@param desired_connection - Developer specificed connection.
	"""

	ports = get_serial_ports()

	CONFIGURATION = check_config(ports)

	
def check_config(ports):
	"""
	Pulls the connected microcontrollers for their name.

	@param ports - Active serial port connections.
	"""

	# COM number.
	port_number = ""
	# Micro-controller descriptor.
	port_description = ""

	# Cycles over all detected ports.
	for port in ports:
		# Parses out port info.
		try:
			number, description = str(port).split("-")
			number = number.strip()
			description = description.strip()
		except:
			print("Unable to parse: " + str(port))

		# Attempts to ping micro controller via serial. 
		try: 
			ser = serial.Serial()
			ser.port = number
			ser.baudrate = 115200
			ser.open()
			ser.close()
			ACTIVE_PORTS.append()
		except (serial.SerialException):
			INVALID_PORTS.append(port)



class serial_object():

	def __init__(self):











def send_serial(message):
	""" 
	Sends passed in parameter over the bound serial port.

	@param message - Paramter to be encoded and sent.
	"""





