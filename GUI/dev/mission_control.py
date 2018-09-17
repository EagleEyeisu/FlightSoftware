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
from communication import *
from inputs import get_gamepad
import globals as g


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
		self.target_throttle_set = 0
		self.target_altitude = 0
		self.target_altitude_set = 0
		self.target_latitude = 0
		self.target_latitude_set = 0
		self.target_longitude = 0
		self.target_longitude_set = 0

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
		self.target_throttle_set = StringVar(self.mc_frame)
		self.target_altitude = StringVar(self.mc_frame)
		self.target_altitude_set = StringVar(self.mc_frame)
		self.target_latitude = StringVar(self.mc_frame)
		self.target_latitude_set = StringVar(self.mc_frame)
		self.target_longitude = StringVar(self.mc_frame)
		self.target_longitude_set = StringVar(self.mc_frame)
		self.authority_mode = StringVar(self.mc_frame)
		self.modified_commands = StringVar(self.mc_frame)

		# Configures tracing for all variables.
		self.operational_mode.trace("w", self.callback_update_transmission)
		self.roll_call_status.trace("w", self.callback_update_transmission)
		self.craft_anchor.trace('w', self.callback_update_transmission)
		self.target_throttle.trace("w", self.callback_update_transmission)
		self.target_altitude.trace("w", self.callback_update_transmission)
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
		self.craft_anchor.set("DROPPED")
		self.craft_received_id.set("-------")
		self.home_time.set("-------")
		self.target_throttle.set("0")
		self.target_throttle_set.set("")
		self.target_altitude.set("-------") # Hook this up
		self.target_latitude.set("-------")
		self.target_longitude.set("-------")
		self.authority_mode.set("MANUAL")
		self.modified_commands.set("")

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
		self.entry_target_throttle_set = Entry(self.mc_frame, textvariable=self.target_throttle_set, justify='right')
		self.entry_target_altitude = Entry(self.mc_frame, state="readonly", textvariable=self.target_altitude, justify='right')
		self.entry_target_altitude_set =  Entry(self.mc_frame, textvariable=self.target_altitude_set, justify='right')
		self.entry_target_latitude = Entry(self.mc_frame, state="readonly", textvariable=self.target_latitude, justify='right')
		self.entry_target_latitude_set = Entry(self.mc_frame, textvariable=self.target_latitude_set, justify='right')
		self.entry_target_longitude = Entry(self.mc_frame, state="readonly", textvariable=self.target_longitude, justify='right')
		self.entry_target_longitude_set = Entry(self.mc_frame, textvariable=self.target_longitude_set, justify='right')
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
		self.button_roll_call_start = Button(self.mc_frame, text="RC Start", command=self.callback_roll_call_start)
		self.button_roll_call_stop = Button(self.mc_frame, text="RC Stop", command=self.callback_roll_call_stop)
		self.button_start_network = Button(self.mc_frame, text="Network Start", command=self.callback_network_start)
		self.button_anchor_set = Button(self.mc_frame, text="Drop Anchor!", command=self.callback_craft_anchor)
		self.button_target_throttle_set = Button(self.mc_frame, text="Set", command=self.callback_target_throttle)
		self.button_target_altitude_set = Button(self.mc_frame, text="Set", command=self.callback_target_altitude)
		self.button_target_latitude_set = Button(self.mc_frame, text="Set", command=self.callback_target_latitude)
		self.button_target_longitude_set = Button(self.mc_frame, text="Set", command=self.callback_target_longitude)
		self.button_queue_commands = Button(self.mc_frame, text="Send", command=self.callback_queue_commands)

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
		self.button_roll_call_start.grid(row=3, column=0, rowspan=2, sticky='nes')
		self.button_roll_call_stop.grid(row=3, column=1, rowspan=2, sticky='ns')
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

	def main_mc_tab(self):
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

		# Configures serial environment.
		setup_comms()

	def callback_config_controller(self):
		"""
		Responsible for connecting to and monitoring the threaded processes for manual control.
		(STARTS XBOX CONTROLLER / CONNECTS / MONITORS USER'S INPUT)
		
		@param self - Instance of the class.
		"""

		#thread_manual_input = threading.Thread(target=manual_mode_monitor, args=())
		#thread_manual_input.start()

	def manual_mode_monitor(self):
		"""
		Threaded method (its always running). 
		This method monitors the changes in the authority mode (manual vs auto).
		When in manual, this method reads an xbox controller's input every 3 seconds.

		@param self - Instance of the class.
		"""

		while(1):
			try:
				# Checks for a incoming data.
				if self.authority_mode.get() is "MANUAL":
					junk = 1 + 1
			except:
				pass

	def callback_update_gui(self):
		"""
		Method is run each time the connected microcontrollers send serial data to the gui.
		This method is triggered via a .trace() on the StringVar object .input which
		can be found near the bottom of the communication.py file.

		@param self - Instance of the class.
		"""

		temp_input = ""

		if g.PORT_MC_LORA is not None:
			temp_input = g.PORT_MC_LORA.input.get()
			if "N" in temp_input:
				serial_data, radio_data = str(temp_input).split("[")
				# Variables such as '$' and 'N' are thrown out as junk.
				# t_ stands for temp because the numbers need to be converted to the 
				# corresponding string for the gui to show.
				junk, junk, t_anchor, t_alt, t_lat, t_lon, t_event = str(serial_data).split(",")
				t_radio_in, t_radio_out, junk = str(radio_data).split("/")

		if g.PORT_CRAFT_LORA is not None:
			placeholder = 1 + 1

		if g.PORT_CRAFT_MEGA is not None:
			placeholder = 1 + 1

	def callback_update_transmission(self, *args):
		"""
		Updates the StringVar used to show user changes in the GUI. This is also the variable
		that is sent back to the mission_control microcontroller when "Send" is pressed.

		@param self  - Instance of the class.
		@param *args - Any other random system varaible that gets passed in.
		"""

		temp_packet = ""

		try:
			# Checks for normal serial operations.
			if self.authority_mode.get() == "AUTO":
				temp_packet += "$"
				temp_packet += ","
				temp_packet += str(self.authority_mode.get())
				temp_packet += ","
				temp_packet += str(self.operational_mode.get())
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
				temp_packet += "$"

			# Checks for roll call serial operations.
			elif self.authority_mode.get() == "MANUAL":
				temp_packet += "$"
				temp_packet += ","
				temp_packet += str(self.authority_mode.get())
				temp_packet += ","
				temp_packet += "direction_placeholder"  # str(self.convert_direction())
				temp_packet += ","
				temp_packet += str(self.craft_anchor.get())
				temp_packet += ","
				temp_packet += str(self.target_throttle.get())
				temp_packet += ","
				temp_packet += str(self.operational_mode.get())
				temp_packet += ","
				temp_packet += "$"

			# Sets the variable to the update packet.
			self.modified_commands.set(temp_packet)

		# Prints exeception status.
		except Exception as e:
			# Prints general error statement. (Used to tell which method errored out)
			print("Unable to change packet type.")
			# Prints actual error.
			print("Exception: " + str(e))

	def callback_roll_call_start(self):
		"""
		Triggered by the press of "button_roll_call_start".

		@param self - Instance of the class.
		"""

		self.roll_call_status.set("RUNNING")
		self.operational_mode.set("ROLLCALL")

	def callback_roll_call_stop(self):
		"""
		Triggered by the press of "button_roll_call_stop".

		@param self - Instance of the class.
		"""

		self.roll_call_status.set("FINISHED")
		self.operational_mode.set("STANDBY")

	def callback_network_start(self):
		"""
		Triggered by the press of "button_network_start".

		@param self - Instance of the class.
		"""

		self.operational_mode.set("RUNNING")

	def callback_craft_anchor(self):
		"""
		Triggered by the press of "button_craft_anchor".

		@param self - Instance of the class.
		"""

		# Checks for craft's anchor status (THIS IS AN E-BRAKE).
		if self.craft_anchor.get() == "DROPPED":
			# Pulls the anchor back in.
			self.craft_anchor.set("HOISTED")
		else:
			# Drops the anchor!
			self.craft_anchor.set("DROPPED")
			# Sets motor throttle to 0 %.
			self.target_throttle.set("0")

	def callback_target_throttle(self):
		"""
		Triggered by the press of Add button next to target throttle.

		@param self       - Instance of the class.
		"""

		# Upper restriction.
		if int(self.target_throttle_set.get()) > 100:
			self.target_throttle_set.set("100")

		self.target_throttle.set(self.target_throttle_set.get())

	def callback_target_altitude(self):
		"""
		Triggered by the press of Add button next to target altitude.

		@param self       - Instance of the class.
		"""

		self.target_altitude.set(self.target_altitude_set.get())

	def callback_target_latitude(self):
		"""
		Triggered by the press of Add button next to target latitude.

		@param self     - Instance of the class.
		"""

		self.target_latitude.set(self.target_latitude_set.get())

	def callback_target_longitude(self):
		"""
		Triggered by the press of Add button next to target longitude.

		@param self     - Instance of the class.
		"""

		self.target_longitude.set(self.target_longitude_set.get())

	def callback_queue_commands(self):
		"""
		Triggered by the press of SEND button next to the modified commands entry.

		@param self         - Instance of the class.
		"""

		try:
			# Checks for non-null connection to mission control's lora microcontroller.
			if g.PORT_MC_LORA is not None:

				# Converts substring into each's corresponding integer value.
				converted_transmission = self.convert_serial()

				print("Commands: " + str(converted_transmission))
				# If non-null, send transmission via serial port.
				send(g.PORT_MC_LORA.get_port(), converted_transmission)
		# Null connection.
		except Exception as e:
			# Prints general error statement. (Used to tell which method errored out)
			print("Invalid connection to mission control's lora.")
			print("Exception: " + str(e))

	def convert_serial(self):
		"""
		Responsible for taking the variables to be set via serial and converting 
		them to their correct integer value.

		@param self - Instance of the class.
		"""

		new_packet = ""

		try:
			# Checks for normal serial operations.
			if self.authority_mode.get() == "AUTO":
				new_packet += "$"
				new_packet += ","
				new_packet += str(self.convert_authority())
				new_packet += ","
				new_packet += str(self.convert_op_mode())
				new_packet += ","
				new_packet += str(self.convert_anchor())
				new_packet += ","
				new_packet += str(self.target_throttle.get())
				new_packet += ","
				new_packet += str(self.target_altitude.get())
				new_packet += ","
				new_packet += str(self.target_latitude.get())
				new_packet += ","
				new_packet += str(self.target_longitude.get())
				new_packet += ","
				new_packet += "$"
				# Returns new packet.
				return new_packet

			# Checks for roll call serial operations.
			elif self.authority_mode.get() == "MANUAL":
				print("in manual")
				new_packet += "$"
				new_packet += ","
				new_packet += str(self.convert_authority())
				new_packet += ","
				new_packet += "direction"  # str(self.convert_direction())
				new_packet += ","
				new_packet += str(self.convert_anchor())
				new_packet += ","
				new_packet += str(self.target_throttle.get())
				new_packet += ","
				new_packet += str(self.convert_op_mode())
				new_packet += ","
				new_packet += "$"
				# Returns new packet.
				return new_packet

		# Prints exception handler.
		except Exception as e:
			# Prints general error statement. (Used to tell which method errored out)
			print("Unable to convert commands.")
			print("Exception: " + str(e))


	def convert_authority(self):
		"""
		Converts to correct integer value.

		@param self - Instance of the class.
		"""

		if self.authority_mode.get() == "MANUAL":
			return float(0)
		elif self.authority_mode.get() == "AUTO":
			return float(1)

	def convert_op_mode(self):
		"""
		Converts to correct integer value.

		@param self - Instance of the class.
		"""

		if self.operational_mode.get() == "NOT STARTED":
			return float(0)
		elif self.operational_mode.get() == "ROLLCALL":
			return float(1)
		elif self.operational_mode.get() == "STANDBY":
			return float(2)
		elif self.operational_mode.get() == "NORMAL":
			return float(3)

	def convert_anchor(self):
		"""
		Converts to correct integer value.

		@param self - Instance of the class.
		"""

		if self.craft_anchor.get() == "DROPPED":
			return float(0)
		elif self.craft_anchor.get() == "HOISTED":
			return float(1)

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