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

	login = Terminal()
	login.set_credentials("mavric","sucks")
	login.configure_login_window()

# Forces script to start at the method main().
if __name__ == "__main__":
   main()
