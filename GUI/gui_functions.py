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
		self.craft_received_id = None

		# Mission Control variables.
		self.home_time = 0
		self.target_throttle = 0
		self.target_altitude = 0 # Hook this up
		self.target_latitude = 0
		self.target_longitude = 0

		# Network Admin variables.
		self.craft_anchor = 0  # Used for emergency stop (0 normal, 1 = STOP ALL MOVEMENT)
		self.authority_mode = "MANUAL"


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
		
		# Creates and defines the notebook object.
		self.configure_notebook()

		self.create_mission_control_widgets()

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
		theme.configure('TFrame', background="#707372")

		book = Notebook(self.gui_window)
		book.pack(side=LEFT, fill=BOTH, expand=True)

		self.mc_frame = Frame(book)
		self.craft_frame = Frame(book)

		self.mc_frame.columnconfigure((0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19), weight=1)
		self.mc_frame.rowconfigure((0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22), weight=1)

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
		self.craft_time = StringVar(self.mc_frame)
		self.craft_altitude = StringVar(self.mc_frame)
		self.craft_latitude = StringVar(self.mc_frame)
		self.craft_longitude = StringVar(self.mc_frame)
		self.craft_event = StringVar(self.mc_frame)
		self.craft_anchor = StringVar(self.mc_frame)
		self.craft_received_id = StringVar(self.mc_frame)
		self.home_time = StringVar(self.mc_frame)
		self.target_throttle = StringVar(self.mc_frame)
		self.target_altitude = StringVar(self.mc_frame) # Hook this up
		self.target_latitude = StringVar(self.mc_frame)
		self.target_longitude = StringVar(self.mc_frame)
		self.authority_mode = StringVar(self.mc_frame)

		# Initialization of varaibles on GUI startup.
		self.operational_mode.set("NOT STARTED")
		self.roll_call_status.set("NOT STARTED")
		self.radio_received.set("-------")
		self.radio_sent.set("-------")
		self.craft_time.set("-------")
		self.craft_altitude.set("-------")
		self.craft_latitude.set("-------")
		self.craft_longitude.set("-------")
		self.craft_event.set("-------")
		self.craft_anchor.set("-------")
		self.craft_received_id.set("-------")
		self.home_time.set("-------")
		self.target_throttle.set("-------")
		self.target_altitude.set("-------") # Hook this up
		self.target_latitude.set("-------")
		self.target_longitude.set("-------")
		self.authority_mode.set("MANUAL")

		# Creates entry widgets for each variable. (Text windows to be placed in the GUI)
		entry_operational_mode = Entry(self.mc_frame, state="readonly", textvariable=self.operational_mode, justify='center')
		entry_roll_call_status = Entry(self.mc_frame, state="readonly", textvariable=self.roll_call_status, justify='center')
		entry_radio_received = Entry(self.mc_frame, state="readonly", textvariable=self.radio_received)
		entry_radio_sent = Entry(self.mc_frame, state="readonly", textvariable=self.radio_sent)
		entry_craft_time = Entry(self.mc_frame, state="readonly", textvariable=self.craft_time, justify='right')
		entry_craft_altitude = Entry(self.mc_frame, state="readonly", textvariable=self.craft_altitude, justify='right')
		entry_craft_latitude = Entry(self.mc_frame, state="readonly", textvariable=self.craft_latitude, justify='right')
		entry_craft_longitude = Entry(self.mc_frame, state="readonly", textvariable=self.craft_longitude, justify='right')
		entry_craft_event = Entry(self.mc_frame, state="readonly", textvariable=self.craft_event, justify='right')
		entry_craft_anchor = Entry(self.mc_frame, state="readonly", textvariable=self.craft_anchor, justify='right')
		entry_craft_received_id = Entry(self.mc_frame, state="readonly", textvariable=self.craft_received_id, justify='center')
		entry_home_time = Entry(self.mc_frame, state="readonly", textvariable=self.home_time, justify='right')
		entry_target_throttle = Entry(self.mc_frame, state="readonly", textvariable=self.target_throttle, justify='right')
		entry_target_throttle_set = Entry(self.mc_frame, justify='right')
		entry_target_altitude = Entry(self.mc_frame, state="readonly", textvariable=self.target_altitude, justify='right')
		entry_target_altitude_set =  Entry(self.mc_frame, justify='right')
		entry_target_latitude = Entry(self.mc_frame, state="readonly", textvariable=self.target_latitude, justify='right')
		entry_target_latitude_set = Entry(self.mc_frame, justify='right')
		entry_target_longitude = Entry(self.mc_frame, state="readonly", textvariable=self.target_longitude, justify='right')
		entry_target_longitude_set = Entry(self.mc_frame, justify='right')
		entry_modified_transmission = Entry(self.mc_frame, justify='right',
														   textvariable=self.craft_anchor)

		# Creates button widgets. (Triggers specified callback method.)
		button_node_mission_control = Button(self.mc_frame, state=DISABLED, text=self.node_mission_control)
		button_node_eagle_eye = Button(self.mc_frame, state=DISABLED, text=self.node_eagle_eye)
		button_node_relay = Button(self.mc_frame, state=DISABLED, text=self.node_relay)
		button_start_roll_call = Button(self.mc_frame, text="RC Start")
		button_stop_roll_call = Button(self.mc_frame, text="RC Stop")
		button_start_network = Button(self.mc_frame, text="Network Start")
		button_emergency_stop = Button(self.mc_frame, text="Emergency Stop", bg="blue")
		button_emergency_stop_reset = Button(self.mc_frame, text="Undo Stop")
		button_anchor_set = Button(self.mc_frame, text="Drop Anchor!")
		button_target_throttle_set = Button(self.mc_frame, text="Set")
		button_target_altitude_set = Button(self.mc_frame, text="Set")
		button_target_latitude_set = Button(self.mc_frame, text="Set")
		button_target_longitude_set = Button(self.mc_frame, text="Set")
		button_queue_transmission = Button(self.mc_frame, text="Send")

		# Creates checkbox objects.
		checkbox_automatic = Checkbutton(self.mc_frame, text="Automatic", variable=self.authority_mode, onvalue="AUTO", offvalue="OFF")
		checkbox_manual = Checkbutton(self.mc_frame, text="Manual      ", variable=self.authority_mode, onvalue="MANUAL", offvalue="OFF")
		
		# Above divider one.
		self.create_label_east(0, 0, self.mc_frame, "Network Status:")
		entry_operational_mode.grid(row=0, column=1, sticky='w')
		self.create_label_east(1, 0, self.mc_frame, "Roll Call Status:")
		entry_roll_call_status.grid(row=1, column=1, sticky='w')
		self.create_label_east(0, 2, self.mc_frame, "Node Status:")
		button_node_mission_control.grid(row=0, column=3, sticky='we')
		button_node_eagle_eye.grid(row=1, column=3, sticky='we')
		button_node_relay.grid(row=0, column=4, sticky='we')
		self.create_label_east(0, 5, self.mc_frame, "Received:")
		entry_radio_received.grid(row=0, column=6, columnspan=14, sticky='we')
		self.create_label_east(1, 5, self.mc_frame, "Sent:")
		entry_radio_sent.grid(row=1, column=6, columnspan=14, stick='we')
		button_start_roll_call.grid(row=3, column=0, rowspan=2, sticky='nes')
		button_stop_roll_call.grid(row=3, column=1, rowspan=2, sticky='ns')
		button_start_network.grid(row=3, column=2, rowspan=2, sticky='nws')
		button_emergency_stop.grid(row=3, column=4, rowspan=2, sticky='nws')
		button_emergency_stop_reset.grid(row=3, column=5, rowspan=2, sticky='ns')
		button_anchor_set.grid(row=3, column=11, rowspan=2, sticky='ns')

		# Terminal divider one.
		terminal_divider_one = Label(self.mc_frame, background="#F1BE48")
		terminal_divider_one.grid(row=5, column=0, columnspan=20, sticky='we')

		# Above divider two.
		self.create_label_center(6, 1, self.mc_frame, "CRAFT")
		self.create_label_center(7, 0, self.mc_frame, "Craft Up Time (s) ")
		entry_craft_time.grid(row=7, column=1, sticky='we')
		self.create_label_center(8, 0, self.mc_frame, "Craft Altitude (m) ")
		entry_craft_altitude.grid(row=8, column=1, sticky='we')
		self.create_label_center(9, 0, self.mc_frame, "Craft Latitude       ")
		entry_craft_latitude.grid(row=9, column=1, sticky='we')
		self.create_label_center(10, 0, self.mc_frame, "Craft Longitude   ")
		entry_craft_longitude.grid(row=10, column=1, sticky='we')
		self.create_label_center(11, 0, self.mc_frame, "Craft Event            ")
		entry_craft_event.grid(row=11, column=1, sticky='we')
		self.create_label_center(12, 0, self.mc_frame, "Anchor Status      ")
		entry_craft_anchor.grid(row=12, column=1, sticky='we')
		# self.create_label_center(?, ?, self.mc_frame, "Craft ID")
		# entry_craft_longitude.grid(row=?, column=?, sticky='we')

		# Terminal divider two.
		terminal_divider_two = Label(self.mc_frame, background="#F1BE48")
		terminal_divider_two.grid(row=13, column=0, columnspan=20, sticky='we')

		# Below divider three.
		self.create_label_center(14, 1, self.mc_frame, "MISSION CONTROL")
		self.create_label_center(15, 0, self.mc_frame, "Home Up Time (s)   ")
		entry_home_time.grid(row=15, column=1, sticky='we')
		self.create_label_center(16, 0, self.mc_frame, "Target Throttle (%) ")
		entry_target_throttle.grid(row=16, column=1, sticky='we')
		entry_target_throttle_set.grid(row=16, column=2, sticky='e')
		button_target_throttle_set.grid(row=16, column=3)
		self.create_label_center(17, 0, self.mc_frame, "Target Altitude (m)  ")
		entry_target_altitude.grid(row=17, column=1, sticky='we')
		entry_target_altitude_set.grid(row=17, column=2, sticky='e')
		button_target_altitude_set.grid(row=17, column=3)
		self.create_label_center(18, 0, self.mc_frame, "Target Latitude        ")
		entry_target_latitude.grid(row=18, column=1, sticky='we')
		entry_target_latitude_set.grid(row=18, column=2, sticky='e')
		button_target_latitude_set.grid(row=18, column=3)
		self.create_label_center(19, 0, self.mc_frame, "Target Longitude    ")
		entry_target_longitude.grid(row=19, column=1, sticky='we')
		entry_target_longitude_set.grid(row=19, column=2, sticky='e')
		button_target_longitude_set.grid(row=19, column=3)
		checkbox_automatic.grid(row=20, column=0)
		checkbox_manual.grid(row=21, column=0)
		self.create_label_east(22, 1, self.mc_frame, "Modified Transmission:")
		entry_modified_transmission.grid(row=22, column=2, columnspan=10, sticky='we')
		button_queue_transmission.grid(row=22, column=13)

	def create_label_east(self, r, c, frame, title):
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

	def create_label_west(self, r, c, frame, title):
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
		label.grid(row=r, column=c, sticky='w')

	def create_label_center(self, r, c, frame, title):
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
		label.grid(row=r, column=c)