#############################################################
#
#	Property of Eagle Eye. 
#
#   Authors:
#           Jared Danner
#
#############################################################
import serial
import sys
import threading

def get_serial_ports():
	""" Detects and returns all active serial ports. """

	return list(serial.tools.list_ports.comports())
