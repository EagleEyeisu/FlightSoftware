#############################################################
#
#	Property of Eagle Eye. 
#
#   Authors:
#           Jared Danner
#
#############################################################

from tkinter import *
from tkinter.ttk import *


class GUI_Terminal():

	def __init__(self):
		""" 
		Initialization function for the class.

		@param self - Instance of the class.
		"""

		self.gui_window = None

	def configure_gui_terminal(self):
		""" 
		Configures the main GUI application window to hold
		the required widgets. Displays upon .mainloop().

		@param self - Instance of the class.
		"""

		self.gui_window = Tk()
		self.gui_window.title("Eagle Eye Serial GUI")
		self.gui_window.configure(background='gray')

		# Dimensions of future TK window.
		window_width = 1400
		window_height = 800

		# Dimensions of current computer monitor.
		screen_width = self.gui_window.winfo_screenwidth()
		screen_height = self.gui_window.winfo_screenheight()

		# Math to move the generation point to have the TK window be centered.
		x_coord = (screen_width/2) - (window_width/2)
		y_coord = (screen_height/2) - (window_height/2)

		# Configures the point the window will display at.
		self.gui_window.geometry("%dx%d+%d+%d" % (window_width, window_height, x_coord, y_coord))

		# Creates and defines the notebook object.
		self.configure_notebook()

		# Displays window.
		self.gui_window.mainloop()

	def configure_notebook(self):
		"""
		Creates and binds the notebook object inside of the GUI terminal.

		@param self - Instance of the class.
		"""

		theme = Style()
		theme.configure('TNotebook', tabposition='wn')
		theme.configure('TNotebook.Tab', width=15)

		book = Notebook(self.gui_window)

		tab_one = Frame(book)
		tab_two = Frame(book)

		eagle_eye_picture = PhotoImage(file="Eagle Eye Logo.PNG")
		mission_control_picture = PhotoImage(file="Mission Control Logo.PNG")

		book.add(tab_one, text="Mission Control", image=eagle_eye_picture, compound=CENTER)
		book.add(tab_two, text="Eagle Eye", image=mission_control_picture, compound=CENTER)
		book.pack(side=LEFT, fill=BOTH)