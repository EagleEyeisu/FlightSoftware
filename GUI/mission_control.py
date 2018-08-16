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
import threading

# Node ID's.
NODE_MISSION_CONTROL_ID = 0
NODE_EAGLE_EYE_ID = 1
NODE_RELAY_ID = 2


class MC_Tab():

	def __init__(self, mc_frame):
		""" 
		Initialization function for the class.

		@param self     - Instance of the class.
		@param mc_frame - Where this class will place its widgets.
		"""

		# Parent variables.
		self.mc_frame = mc_frame

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

		# Other
		self.modified_commands = None

		# Network Admin variables.
		self.craft_anchor = 0  # Used for emergency stop (0 normal, 1 = STOP ALL MOVEMENT)
		self.authority_mode = "MANUAL"

	def variable_setup(self):
		""" 
		Initializes classe variables to proper types and starting values. 

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
		self.modified_commands = StringVar(self.mc_frame)

		# Configures tracing for all variables.
		self.operational_mode.trace("w", self.callback_update_transmission)
		self.roll_call_status.trace("w", self.callback_update_transmission)
		self.craft_anchor.trace('w', self.callback_update_transmission)
		self.target_throttle.trace("w", self.callback_update_transmission)
		self.target_altitude.trace("w", self.callback_update_transmission) # Hook this up
		self.target_latitude.trace("w", self.callback_update_transmission)
		self.target_longitude.trace("w", self.callback_update_transmission)
		self.authority_mode.trace("w", self.callback_update_transmission)

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
		self.modified_commands.set("-------")

	def create_entry_objects(self):
		""" 
		Creates/configures entry objects.

		@param self - Instance of the class.
		"""

		# Creates entry widgets for each variable. (Text windows to be placed in the GUI)
		self.entry_operational_mode = Entry(self.mc_frame, state="readonly", textvariable=self.operational_mode, justify='center')
		self.entry_roll_call_status = Entry(self.mc_frame, state="readonly", textvariable=self.roll_call_status, justify='center')
		self.entry_radio_received = Entry(self.mc_frame, state="readonly", textvariable=self.radio_received)
		self.entry_radio_sent = Entry(self.mc_frame, state="readonly", textvariable=self.radio_sent)
		self.entry_craft_time = Entry(self.mc_frame, state="readonly", textvariable=self.craft_time, justify='right')
		self.entry_craft_altitude = Entry(self.mc_frame, state="readonly", textvariable=self.craft_altitude, justify='right')
		self.entry_craft_latitude = Entry(self.mc_frame, state="readonly", textvariable=self.craft_latitude, justify='right')
		self.entry_craft_longitude = Entry(self.mc_frame, state="readonly", textvariable=self.craft_longitude, justify='right')
		self.entry_craft_event = Entry(self.mc_frame, state="readonly", textvariable=self.craft_event, justify='right')
		self.entry_craft_anchor = Entry(self.mc_frame, state="readonly", textvariable=self.craft_anchor, justify='right')
		self.entry_craft_received_id = Entry(self.mc_frame, state="readonly", textvariable=self.craft_received_id, justify='center')
		self.entry_home_time = Entry(self.mc_frame, state="readonly", textvariable=self.home_time, justify='right')
		self.entry_target_throttle = Entry(self.mc_frame, state="readonly", textvariable=self.target_throttle, justify='right')
		self.entry_target_throttle_set = Entry(self.mc_frame, justify='right')
		self.entry_target_altitude = Entry(self.mc_frame, state="readonly", textvariable=self.target_altitude, justify='right')
		self.entry_target_altitude_set =  Entry(self.mc_frame, justify='right')
		self.entry_target_latitude = Entry(self.mc_frame, state="readonly", textvariable=self.target_latitude, justify='right')
		self.entry_target_latitude_set = Entry(self.mc_frame, justify='right')
		self.entry_target_longitude = Entry(self.mc_frame, state="readonly", textvariable=self.target_longitude, justify='right')
		self.entry_target_longitude_set = Entry(self.mc_frame, justify='right')
		self.entry_modified_commands = Entry(self.mc_frame, state="readonly", justify='right', textvariable=self.modified_commands)

	def create_button_objects(self):
		""" 
		Creates/configures button objects.

		@param self - Instance of the class.
		"""

		# Creates button widgets. (Triggers specified callback method.)
		self.button_node_mission_control = Button(self.mc_frame, state=DISABLED, text=self.node_mission_control)
		self.button_node_eagle_eye = Button(self.mc_frame, state=DISABLED, text=self.node_eagle_eye)
		self.button_node_relay = Button(self.mc_frame, state=DISABLED, text=self.node_relay)
		self.button_start_roll_call = Button(self.mc_frame, text="RC Start", command=self.callback_roll_call_start)
		self.button_stop_roll_call = Button(self.mc_frame, text="RC Stop")
		self.button_start_network = Button(self.mc_frame, text="Network Start")
		self.button_emergency_stop = Button(self.mc_frame, text="Emergency Stop")
		self.button_emergency_stop_reset = Button(self.mc_frame, text="Undo Stop")
		self.button_anchor_set = Button(self.mc_frame, text="Drop Anchor!")
		self.button_target_throttle_set = Button(self.mc_frame, text="Set")
		self.button_target_altitude_set = Button(self.mc_frame, text="Set")
		self.button_target_latitude_set = Button(self.mc_frame, text="Set")
		self.button_target_longitude_set = Button(self.mc_frame, text="Set")
		self.button_queue_commands = Button(self.mc_frame, text="Send")

	def create_checkbox_objects(self):
		""" 
		Creates/configures checkbox objects.

		@param self - Instance of the class.
		"""

		# Creates checkbox objects.
		self.checkbox_automatic = Checkbutton(self.mc_frame, text="Automatic", variable=self.authority_mode, onvalue="AUTO", offvalue="OFF")
		self.checkbox_manual = Checkbutton(self.mc_frame, text="Manual      ", variable=self.authority_mode, onvalue="MANUAL", offvalue="OFF")
		
	def layout_network(self):
		""" 
		Binds the sections of widgets related to the network to the top
		portion of the frame.

		@param self - Instance of the class.
		"""

		# Above divider one.
		self.create_label_east(0, 0, self.mc_frame, "Network Status:")
		self.entry_operational_mode.grid(row=0, column=1, sticky='w')
		self.create_label_east(1, 0, self.mc_frame, "Roll Call Status:")
		self.entry_roll_call_status.grid(row=1, column=1, sticky='w')
		self.create_label_east(0, 2, self.mc_frame, "Node Status:")
		self.button_node_mission_control.grid(row=0, column=3, sticky='we')
		self.button_node_eagle_eye.grid(row=1, column=3, sticky='we')
		self.button_node_relay.grid(row=0, column=4, sticky='we')
		self.create_label_east(0, 5, self.mc_frame, "Received:")
		self.entry_radio_received.grid(row=0, column=6, columnspan=14, sticky='we')
		self.create_label_east(1, 5, self.mc_frame, "Sent:")
		self.entry_radio_sent.grid(row=1, column=6, columnspan=14, stick='we')
		self.button_start_roll_call.grid(row=3, column=0, rowspan=2, sticky='nes')
		self.button_stop_roll_call.grid(row=3, column=1, rowspan=2, sticky='ns')
		self.button_start_network.grid(row=3, column=2, rowspan=2, sticky='nws')
		self.button_anchor_set.grid(row=3, column=4, rowspan=2, sticky='nws')

		# Terminal divider. KEEP THIS AT THE BOTTOM OF THIS METHOD.
		terminal_divider_one = Label(self.mc_frame, background="#F1BE48")
		terminal_divider_one.grid(row=5, column=0, columnspan=20, sticky='we')

	def layout_craft(self):
		""" 
		Binds the sections of widgets related to the craft to the middle
		portion of the frame.

		@param self - Instance of the class.
		"""

		# Above divider.
		self.create_label_center(6, 1, self.mc_frame, "CRAFT")
		self.create_label_center(7, 0, self.mc_frame, "Craft Up Time (s) ")
		self.entry_craft_time.grid(row=7, column=1, sticky='we')
		self.create_label_center(8, 0, self.mc_frame, "Craft Altitude (m) ")
		self.entry_craft_altitude.grid(row=8, column=1, sticky='we')
		self.create_label_center(9, 0, self.mc_frame, "Craft Latitude       ")
		self.entry_craft_latitude.grid(row=9, column=1, sticky='we')
		self.create_label_center(10, 0, self.mc_frame, "Craft Longitude   ")
		self.entry_craft_longitude.grid(row=10, column=1, sticky='we')
		self.create_label_center(11, 0, self.mc_frame, "Craft Event            ")
		self.entry_craft_event.grid(row=11, column=1, sticky='we')
		self.create_label_center(12, 0, self.mc_frame, "Anchor Status      ")
		self.entry_craft_anchor.grid(row=12, column=1, sticky='we')
		# self.create_label_center(?, ?, self.mc_frame, "Craft ID")
		# entry_craft_longitude.grid(row=?, column=?, sticky='we')

		# Terminal divider. KEEP THIS AT THE BOTTOM OF THE METHOD.
		terminal_divider_two = Label(self.mc_frame, background="#F1BE48")
		terminal_divider_two.grid(row=13, column=0, columnspan=20, sticky='we')

	def layout_mission_control(self):
		""" 
		Binds the sections of widgets related to mission_control to the bottom
		portion of the frame.

		@param self - Instance of the class.
		"""

		# Below final divider.
		self.create_label_center(14, 1, self.mc_frame, "MISSION CONTROL")
		self.create_label_center(15, 0, self.mc_frame, "Home Up Time (s)   ")
		self.entry_home_time.grid(row=15, column=1, sticky='we')
		self.create_label_center(16, 0, self.mc_frame, "Target Throttle (%) ")
		self.entry_target_throttle.grid(row=16, column=1, sticky='we')
		self.entry_target_throttle_set.grid(row=16, column=2, sticky='e')
		self.button_target_throttle_set.grid(row=16, column=3)
		self.create_label_center(17, 0, self.mc_frame, "Target Altitude (m)  ")
		self.entry_target_altitude.grid(row=17, column=1, sticky='we')
		self.entry_target_altitude_set.grid(row=17, column=2, sticky='e')
		self.button_target_altitude_set.grid(row=17, column=3)
		self.create_label_center(18, 0, self.mc_frame, "Target Latitude        ")
		self.entry_target_latitude.grid(row=18, column=1, sticky='we')
		self.entry_target_latitude_set.grid(row=18, column=2, sticky='e')
		self.button_target_latitude_set.grid(row=18, column=3)
		self.create_label_center(19, 0, self.mc_frame, "Target Longitude    ")
		self.entry_target_longitude.grid(row=19, column=1, sticky='we')
		self.entry_target_longitude_set.grid(row=19, column=2, sticky='e')
		self.button_target_longitude_set.grid(row=19, column=3)
		self.checkbox_automatic.grid(row=20, column=0)
		self.checkbox_manual.grid(row=21, column=0)
		self.create_label_east(22, 1, self.mc_frame, "Modified Commands:")
		self.entry_modified_commands.grid(row=22, column=2, columnspan=6, sticky='we')
		self.button_queue_commands.grid(row=22, column=9)

	def populate_mc_tab(self):
		"""
		Fills the frame with widgets needed for the mission control interface.

		@param self - Instance of the class.
		"""

		# Initializes class variables.
		self.variable_setup()
		# Creates/configures the tk widgets. 
		self.create_entry_objects()
		self.create_button_objects()
		self.create_checkbox_objects()
		# Aligns the widgets to the frame's grid.
		self.layout_network()
		self.layout_craft()
		self.layout_mission_control()

	def callback_update_transmission(self, *args):
		"""
		Updates the StringVar used to show user changes in the GUI. This is also the variable
		that is sent back to the mission_control microcontroller when "Send" is pressed.

		@param self  - Instance of the class.
		@param *args - Any other random system varaible that gets passed in.
		"""

		temp_packet = ""

		temp_packet += str(self.operational_mode.get())
		temp_packet += ","
		temp_packet += str(self.roll_call_status.get())
		temp_packet += ","
		temp_packet += str(self.craft_anchor.get())
		temp_packet += ","
		temp_packet += str(self.target_throttle.get())
		temp_packet += ","
		temp_packet += str(self.target_altitude.get())
		temp_packet += ","
		temp_packet += str(self.target_latitude.get())
		temp_packet += ","
		temp_packet += str(self.target_longitude.get())
		temp_packet += ","
		temp_packet += str(self.authority_mode.get())

		self.modified_commands.set(temp_packet)

	def callback_roll_call_start(self):
		"""
		Triggered by the press of "button_start_roll_call".

		@param self - Instance of the class.
		"""

		self.roll_call_status.set("RUNNING")

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