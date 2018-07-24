#############################################################
#
#	Property of Eagle Eye. 
#
#   Authors:
#           Jared Danner
#
#############################################################
from tkinter import *
from terminal_functions import *

LOGIN_ENABLE = True


def main():
	"""  Main method of GUI """
	
	if LOGIN_ENABLE:
		login()

	

def login():
	"""  Login Screen Config and Generation """

	# Creation of class.
	login = Login_Terminal()
	# Sets the password.
	login.set_credentials("test")
	# Configures and displays the login window.
	login.configure_login_window()

# Forces script to start at the method main().
if __name__ == "__main__":
   main()
