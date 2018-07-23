#############################################################
#
#	Property of Eagle Eye. 
#
#   Authors:
#           Jared Danner
#
#############################################################
from tkinter import *


class Terminal():

	def __init__(self):
		""" 
		Initialization function for the class.

		@param self - Instance of the class.
		"""

		self.username = ""
		self.password = ""
		self.login_window = None

	def set_credentials(self, username, password):
		""" 
		Sets the developer's desired user and pass.

		@param self     - Instance of the class.
		@param username - Username to be set.
		@param password - Password to be set.
		"""

		self.username = username
		self.password = password

	def callback_verify_credentials(self):
		""" 
		Configures the login window to hold the appropriate
		buttons and entries.

		@param self - Instance of the class.
		"""

		junk = 1 + 1

	def configure_login_window(self):
		""" 
		Configures the login window to hold the appropriate
		buttons and entries.

		@param self - Instance of the class.
		"""

		self.login_window = Tk()
		self.login_window.title("Verify Credentials")
		self.login_window.geometry('300x200')

		Label(self.login_window, text="Username: ").grid(row=4)
		Label(self.login_window, text="Password: ").grid(row=5)

		entry_username = Entry(self.login_window)
		entry_password = Entry(self.login_window)

		entry_username.grid(row=4, column=1)
		entry_password.grid(row=5, column=1)

		Button(self.login_window, text='Quit', command=self.login_window.quit).grid(row=6, column=3, sticky=W, pady=4)
		Button(self.login_window, text='Login', command=self.callback_verify_credentials).grid(row=6, column=1, sticky=W, pady=4)

		self.login_window.mainloop()
