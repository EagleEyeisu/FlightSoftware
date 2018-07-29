#############################################################
#
#	Property of Eagle Eye. 
#
#   Authors:
#           Jared Danner
#
#############################################################

from tkinter import *

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

		# Picture.
		eagle_eye_picture = PhotoImage(file="Eagle Eye Logo.PNG")
		title_picture = Label(self.gui_window, image=eagle_eye_picture)
		title_picture.pack()

		self.gui_window.mainloop()