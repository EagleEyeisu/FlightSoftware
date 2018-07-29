#############################################################
#
#	Property of Eagle Eye. 
#
#   Authors:
#           Jared Danner
#
#############################################################
from tkinter import *
from login_functions import *

LOGIN_ENABLE = True


def main():
	"""  Main method of GUI """
	
	# Checks for login method.
	if LOGIN_ENABLE:
		login()

	# Launches the main GUI terminal.
	GUI()

def login():
	"""  Login Screen Config and Generation """

	# Creation of class.
	login = Login_Terminal()
	# Sets the password.
	login.set_credentials("test")
	# Configures and displays the login window.
	login.configure_login_window()

def GUI():



# Forces script to start at the method main().
if __name__ == "__main__":
   main()
