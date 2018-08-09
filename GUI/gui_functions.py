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

# Node ID's.
NODE_MISSION_CONTROL_ID = 0
NODE_EAGLE_EYE_ID = 1
NODE_RELAY_ID = 2

class GUI_Terminal():

	def __init__(self):
		""" 
		Initialization function for the class.

		@param self - Instance of the class.
		"""

		self.gui_window = None
		self.mc_frame = None
		self.craft_frame = None

		# Network variables.
		self.operational_mode = None
		self.roll_call_status = None
		self.network_nodes = []  # May not use
		self.node_mission_control = None
		self.node_eagle_eye = None
		self.node_relay = None
		self.radio_received = None
		self.radio_sent = None

		# Eagle Eye Craft variables.
		self.craft_time = 0
		self.craft_altitude = 0
		self.craft_latitude = 0
		self.craft_longitude = 0
		self.craft_event = 0
		self.received_craft_id = None

		# Mission Control variables.
		self.home_time = 0
		self.target_throttle = 0
		self.target_altitude = 0 # Hook this up
		self.target_latitude = 0
		self.target_longitude = 0

		# Network Admin variables.
		self.freeze = 0  # Used for emergency stop (0 normal, 1 = STOP ALL MOVEMENT)
		self.manual_control = False


	def configure_gui_terminal(self):
		""" 
		Configures the main GUI application window to hold
		the required widgets. Displays upon .mainloop().

		@param self - Instance of the class.
		"""

		self.gui_window = Tk()
		self.gui_window.title("Eagle Eye Serial GUI")
		self.gui_window.configure(background='gray')
		self.gui_window.attributes('-fullscreen', True)
		self.gui_window.bind("<Escape>", lambda e: self.gui_window.quit())

		# Dimensions of future TK window.
		# window_width = 1400
		# window_height = 800
		# Dimensions of current computer monitor.
		# screen_width = self.gui_window.winfo_screenwidth()
		# screen_height = self.gui_window.winfo_screenheight()
		# Math to move the generation point to have the TK window be centered.
		# x_coord = (screen_width/2) - (window_width/2)
		# y_coord = (screen_height/2) - (window_height/2)
		# Configures the point the window will display at.
		# self.gui_window.geometry("%dx%d+%d+%d" % (window_width, window_height, x_coord, y_coord))

		#self.connection_status = label
		
		# Creates and defines the notebook object.
		self.configure_notebook()

		self.create_mission_control_widgets()

		# Sets focus on connection banner to allow bring the PC attention to the program.
		self.connection_status.focus()

		# Displays window.
		self.gui_window.mainloop()

	def configure_notebook(self):
		"""
		Creates and binds the notebook object inside of the GUI terminal.

		@param self - Instance of the class.
		"""

		theme = Style()
		theme.configure('TNotebook', tabposition='wn')
		theme.configure('TNotebook.Tab', padding=10)
		theme.configure('TNotebook', background='gray60')

		book = Notebook(self.gui_window)
		book.pack(side=LEFT, fill=BOTH, expand=True)

		self.mc_frame = Frame(book)
		self.craft_frame = Frame(book)

		self.mc_frame.columnconfigure((0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19), weight=1)
		self.mc_frame.rowconfigure((0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17), weight=0)

		book.add(self.mc_frame, text="Mission Control")
		book.add(self.craft_frame, text="     Eagle Eye       ")
		
	def create_mission_control_widgets(self):
		"""
		Fills the frame with widgets needed for the mission control interface.

		@param self - Instance of the class.
		"""

		# StringVar variables are strings that update their text in entries when
		# their value is changed.
		self.operational_mode = StringVar(self.mc_frame)
		self.roll_call_status = StringVar(self.mc_frame)
		self.node_mission_control = "HOME"
		self.node_eagle_eye = "CRAFT"
		self.node_relay = "RELAY"
		self.radio_received = StringVar(self.mc_frame)
		self.radio_sent = StringVar(self.mc_frame)

		self.operational_mode.set("NOT STARTED")
		self.roll_call_status.set("NOT STARTED")
		self.radio_received = ""
		self.radio_sent = ""

		# Creates entry widgets for each variable. (Text windows to be placed in the GUI)
		entry_operational_mode = Entry(self.mc_frame, state="readonly", textvariable=self.operational_mode)
		entry_roll_call_status = Entry(self.mc_frame, state="readonly", textvariable=self.roll_call_status)
		entry_radio_received = Entry(self.mc_frame, state="readonly", textvariable=self.radio_received)
		entry_radio_sent = Entry(self.mc_frame, state="readonly", textvariable=self.radio_sent)

		# Creates button widgets. (Triggers)
		button_node_mission_control = Button(self.mc_frame, state=DISABLED, text=self.node_mission_control)
		button_node_eagle_eye = Button(self.mc_frame, state=DISABLED, text=self.node_eagle_eye)
		button_node_relay = Button(self.mc_frame, state=DISABLED, text=self.node_relay)
		button_start_roll_call = Button(self.mc_frame, text="RC Start")
		button_start_roll_call = Button(self.mc_frame, text="RC Stop")
		button_start_roll_call = Button(self.mc_frame, text="Network Start")

		self.create_label(0, 0, self.mc_frame, "Network Status:")
		entry_operational_mode.grid(row=0, column=1, sticky='w')

		self.create_label(1, 0, self.mc_frame, "Roll Call Status:")
		entry_roll_call_status.grid(row=1, column=1, sticky='w')

		self.create_label(0, 2, self.mc_frame, "Node Status:")
		button_node_mission_control.grid(row=0, column=3, sticky='we')
		button_node_eagle_eye.grid(row=1, column=3, sticky='we')
		button_node_relay.grid(row=0, column=4, sticky='we')

		self.create_label(0, 5, self.mc_frame, "Received:")
		entry_radio_received.grid(row=0, column=6, columnspan=14, sticky='ew')

		self.create_label(1, 5, self.mc_frame, "Sent:")
		entry_radio_sent.grid(row=1, column=6, columnspan=14, stick='ew')

		# Configures button to appear as a text box.

	def create_label(self, r, c, frame, title):
		"""
		Creates a label to be placed inside of the passed in frame at the given grid locations.

		@param self  - Instance of the class.
		@param r     - Row of frame.
		@param c     - Column of frame.
		@param frame - Frame to be bound to.
		@param title - String title to be assigned to the label.
		"""

		# Label object.
		label = Label(frame, text=title, font='Helvetica 12 bold')
		label.grid(row=r, column=c, sticky='e')