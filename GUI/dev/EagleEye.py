#############################################################
#
#	Property of Eagle Eye. 
#
#   Authors:
#           Robert Zartman
#
#############################################################

from tkinter import *
from tkinter.ttk import *
from communication import *
import os
import globals as g

# Node ID's.
NODE_MISSION_CONTROL_ID = 0
NODE_EAGLE_EYE_ID = 1
NODE_RELAY_ID = 2


class MC_Tabi():

	def __init__(self, craft_frame):
		""" 
		Initialization function for the class.

		@param self     - Instance of the class.
		@param craft_frame - Where this class will place its widgets.
		"""

		# Parent variables.
		self.craft_frame = craft_frame

		# Network variables.
		self.operational_mode = None
		self.roll_call_status = None
		self.node_mission_control = None
		self.node_eagle_eye = None
		self.node_relay = None
		self.radio_received = None
		self.radio_sent = None

		

		# Other
		self.modified_commands = None


                #New Variables
		#Data_h Variables_Flight_Data (for EagleEye Tab)
		self.craft_roll = 0
		self.craft_pitch = 0
		self.craft_yaw = 0
		self.pressure = 0
		self.external_temperature = 0
		self.destination_distance = 0
		self.current_speed = 0
		self.satillite_count = 0

		#IMU_h Variables
		self.target_heading = 0
		self.current_heading = 0
		self.turn_right = None
		self.turn_left = None
		self.move_up = None
		self.move_down = None
		self.move_forward = None

		#Motor_h Variables
		self.current_throttle = 0
		self.throttle_input = 0
		self.servo_degree = 0
		self.craft_state = None

	def variable_setup(self):
		""" 
		Initializes class variables to proper types and starting values. 

		@param self - Instance of the class.
		"""

		# StringVar variables are strings that update their text in entries when
		# their value is changed.
		self.operational_mode = StringVar(self.craft_frame)
		self.roll_call_status = StringVar(self.craft_frame)
		self.node_mission_control = "HOME"
		self.node_eagle_eye = "CRAFT"
		self.node_relay = "RELAY"
		self.radio_received = StringVar(self.craft_frame)
		self.radio_sent = StringVar(self.craft_frame)
		
		
		self.modified_commands = StringVar(self.craft_frame)

		#New Variables
		#Data_h Variables_Flight_Data (for EagleEye Tab)
		self.craft_roll = StringVar(self.craft_frame)
		self.craft_pitch = StringVar(self.craft_frame)
		self.craft_yaw = StringVar(self.craft_frame)
		self.pressure = StringVar(self.craft_frame)
		self.external_temperature = StringVar(self.craft_frame)
		self.destination_distance = StringVar(self.craft_frame)
		self.current_speed = StringVar(self.craft_frame)
		self.satillite_count = StringVar(self.craft_frame)

		#IMU_h Variables
		self.target_heading = StringVar(self.craft_frame)
		self.current_heading = StringVar(self.craft_frame)
		self.turn_right = StringVar(self.craft_frame)
		self.turn_left = StringVar(self.craft_frame)
		self.move_up = StringVar(self.craft_frame)
		self.move_down = StringVar(self.craft_frame)
		self.move_forward = StringVar(self.craft_frame)

		#Motor_h Variables
		self.current_throttle = StringVar(self.craft_frame)
		self.throttle_input = StringVar(self.craft_frame)
		self.servo_degree = StringVar(self.craft_frame)
		self.craft_state = StringVar(self.craft_frame)

                #Configure
		
		# Configures tracing for all variables.
		self.operational_mode.trace("w", self.callback_update_transmission)
		self.roll_call_status.trace("w", self.callback_update_transmission)

		#Configure
		

		#New Variables
		#Data_h Variables_Flight_Data (for EagleEye Tab)
		self.craft_roll = StringVar(self.craft_frame)
		self.craft_pitch = StringVar(self.craft_frame)
		self.craft_yaw = StringVar(self.craft_frame)
		self.pressure = StringVar(self.craft_frame)
		self.external_temperature = StringVar(self.craft_frame)
		self.destination_distance = StringVar(self.craft_frame)
		self.current_speed = StringVar(self.craft_frame)
		self.satillite_count = StringVar(self.craft_frame)

		#IMU_h Variables
		self.target_heading = StringVar(self.craft_frame)
		self.current_heading = StringVar(self.craft_frame)
		self.turn_right = StringVar(self.craft_frame)
		self.turn_left = StringVar(self.craft_frame)
		self.move_up = StringVar(self.craft_frame)
		self.move_down = StringVar(self.craft_frame)
		self.move_forward = StringVar(self.craft_frame)

		#Motor_h Variables
		self.current_throttle = StringVar(self.craft_frame)
		self.throttle_input = StringVar(self.craft_frame)
		self.servo_degree = StringVar(self.craft_frame)
		self.craft_state = StringVar(self.craft_frame)

		# Initialization of varaibles on GUI startup.
		self.operational_mode.set("NOT STARTED")
		self.roll_call_status.set("NOT STARTED")
		self.radio_received.set("-------")
		self.radio_sent.set("-------")
		
		self.modified_commands.set("-------")

		#New Variables
		#Data_h Variables_Flight_Data (for EagleEye Tab)
		self.craft_roll.set("------")
		self.craft_pitch.set("------")
		self.craft_yaw.set("------")
		self.pressure.set("------")
		self.external_temperature.set("------")
		self.destination_distance.set("------")
		self.current_speed.set("------")
		self.satillite_count.set("-----")

		#IMU_h Variables
		self.target_heading.set("------")
		self.current_heading.set("------")
		self.turn_right.set("None")
		self.turn_left.set("None")
		self.move_up.set("None")
		self.move_down.set("None")
		self.move_forward.set("None")

		#Motor_h Variables
		self.current_throttle.set("------")
		self.throttle_input.set("------")
		self.servo_degree.set("------")
		self.craft_state.set("None")

	def create_entry_objects(self):
		""" 
		Creates/configures entry objects.

		@param self - Instance of the class.
		"""

		
		#New Variables
		#Data_h Variables_Flight_Data (for EagleEye Tab)
		self.entry_craft_roll = Entry(self.craft_frame, state="readonly", textvariable = self.craft_roll, justify='center')
		self.entry_craft_pitch = Entry(self.craft_frame, state="readonly", textvariable = self.craft_pitch, justify='center')
		self.entry_craft_yaw = Entry(self.craft_frame, state="readonly", textvariable = self.craft_yaw, justify='center')
		self.entry_pressure = Entry(self.craft_frame, state="readonly", textvariable = self.pressure, justify='center')
		self.entry_external_temperature = Entry(self.craft_frame, state="readonly", textvariable = self.external_temperature, justify='center')
		self.entry_destination_distance = Entry(self.craft_frame, state="readonly", textvariable = self.destination_distance, justify='center')
		self.entry_current_speed = Entry(self.craft_frame, state="readonly", textvariable = self.current_speed, justify='center')
		self.entry_satillite_count = Entry(self.craft_frame, state="readonly", textvariable = self.satillite_count, justify='center')

		#IMU_h Variables
		self.entry_target_heading = Entry(self.craft_frame, state="readonly", textvariable = self.target_heading, justify='center')
		self.entry_current_heading = Entry(self.craft_frame, state="readonly", textvariable = self.current_heading, justify='center')
		self.entry_turn_right = Entry(self.craft_frame, state="readonly", textvariable = self.turn_right, justify='center')
		self.entry_turn_left = Entry(self.craft_frame, state="readonly", textvariable = self.turn_left, justify='center')
		self.entry_move_up = Entry(self.craft_frame, state="readonly", textvariable = self.move_up, justify='center')
		self.entry_move_down = Entry(self.craft_frame, state="readonly", textvariable = self.move_down, justify='center')
		self.entry_move_forward = Entry(self.craft_frame, state="readonly", textvariable = self.move_forward, justify='center')

		#Motor_h Variables
		self.entry_current_throttle = Entry(self.craft_frame, state="readonly", textvariable = self.current_throttle, justify='center')
		self.entry_throttle_input = Entry(self.craft_frame, state="readonly", textvariable = self.throttle_input, justify='center')
		self.entry_servo_degree = Entry(self.craft_frame, state="readonly", textvariable = self.servo_degree, justify='center')
		self.entry_craft_state = Entry(self.craft_frame, state="readonly", textvariable = self.craft_state, justify='center')

	def create_button_objects(self):
		""" 
		Creates/configures button objects.

		@param self - Instance of the class.
		"""

		# Creates button widgets. (Triggers specified callback method.)
		
		self.button_queue_commands = Button(self.craft_frame, text="Get Data", command=self.callback_queue_commands)

		#New Button
		self.button_update = Button(self.craft_frame, text="Update", command=self.callback_update)

	def create_checkbox_objects(self):
		""" 
		Creates/configures checkbox objects.

		@param self - Instance of the class.
		"""

		# Creates checkbox objects.
		
		
	def layout_network(self):
		""" 
		Binds the sections of widgets related to the network to the top
		portion of the frame.

		@param self - Instance of the class.
		"""

		# Above divider one.
		
		self.create_label_east(0,0, self.craft_frame, "Roll")
		self.entry_craft_roll.grid(row=0, column=1, sticky = 'w')
		self.create_label_east(1,0, self.craft_frame, "Pitch")
		self.entry_craft_pitch.grid(row=1, column=1, sticky = 'w')
		self.create_label_east(2,0, self.craft_frame, "Yaw")
		self.entry_craft_yaw.grid(row=2, column=1, sticky = 'w')
		self.create_label_east(0,5, self.craft_frame, "Pressure")
		self.entry_pressure.grid(row=0, column=6, sticky = 'w')
		self.create_label_east(1,5, self.craft_frame, "External Temperature")
		self.entry_external_temperature.grid(row=1, column=6, sticky = 'w')
		self.create_label_east(2,5, self.craft_frame, "Destination Distance")
		self.entry_destination_distance.grid(row=2, column=6, sticky = 'w')
		self.create_label_east(3,5, self.craft_frame, "Current Speed")
		self.entry_current_speed.grid(row=3, column=6, sticky = 'w')
		

		# Terminal divider. KEEP THIS AT THE BOTTOM OF THIS METHOD.
		terminal_divider_one = Label(self.craft_frame, background="#F1BE48")
		terminal_divider_one.grid(row=5, column=0, columnspan=20, sticky='we')

	def layout_craft(self):
		""" 
		Binds the sections of widgets related to the craft to the middle
		portion of the frame.

		@param self - Instance of the class.
		"""

		# Above divider.

		
		self.create_label_center(6, 1, self.craft_frame, "Target Heading")
		self.entry_target_heading.grid(row = 6,column = 2, sticky = 'we')
		self.create_label_center(7, 1, self.craft_frame, "Current Heading")
		self.entry_current_heading.grid(row = 7,column = 2, sticky = 'we')
		self.create_label_center(8, 1, self.craft_frame, "Turn Right")
		self.entry_turn_right.grid(row = 8,column = 2, sticky = 'we')
		self.create_label_center(9, 1, self.craft_frame, "Turn Left")
		self.entry_turn_left.grid(row = 9,column = 2, sticky = 'we')
		self.create_label_center(6, 5, self.craft_frame, "Move Up")
		self.entry_move_up.grid(row = 6,column = 6, sticky = 'we')
		self.create_label_center(7, 5, self.craft_frame, "Move Down")
		self.entry_move_down.grid(row = 7,column = 6, sticky = 'we')
		self.create_label_center(8, 5, self.craft_frame, "Move Forward")
		self.entry_move_forward.grid(row = 8,column = 6, sticky = 'we')
                

		# Terminal divider. KEEP THIS AT THE BOTTOM OF THE METHOD.
		terminal_divider_two = Label(self.craft_frame, background="#F1BE48")
		terminal_divider_two.grid(row=13, column=0, columnspan=20, sticky='we')

	def layout_mission_control(self):
		""" 
		Binds the sections of widgets related to mission_control to the bottom
		portion of the frame.

		@param self - Instance of the class.
		"""

		# Below final divider.
		'''
		
                '''
		self.create_label_center(14,0, self.craft_frame, "Current Throttle")
		self.entry_current_throttle.grid(row=14, column=1, sticky = 'w')
		self.create_label_center(14,3, self.craft_frame, "Throttle Input")
		self.entry_throttle_input.grid(row=14, column=4, sticky = 'w')
		self.create_label_center(15,0, self.craft_frame, "Servo Degree")
		self.entry_servo_degree.grid(row=15, column=1, sticky = 'w')
		self.create_label_center(15,3, self.craft_frame, "Craft State")
		self.entry_craft_state.grid(row=15, column=4, sticky = 'w')
		self.create_label_center(14,6, self.craft_frame, "Satillite Count")
		self.entry_satillite_count.grid(row=14, column=7, sticky = 'w')

		self.button_queue_commands.grid(row = 16, column = 4, sticky = 'e')
		self.button_update.grid(row = 16, column = 3, sticky = 'e')
		
	def main_mc_tabi(self):
		"""
		Fills the frame with widgets needed for the mission control interface.

		@param self - Instance of the class.
		"""

		# Configures serial environment.
		setup_comms()

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

		# Setup threads to monitor for manual input.
		self.control_setup()

	def control_setup(self):
		"""
		Responsible for monitoring the threaded processes for manual control.
		(PICKED UP XBOX CONTROLLER INPUT)
		
		@param self - Instance of the class.
		"""

		#thread_manual_input = threading.Thread(target=manual_mode_monitor, args=())
		#thread_manual_input.start()

	def callback_update_transmission(self, *args):
		"""
		Updates the StringVar used to show user changes in the GUI. This is also the variable
		that is sent back to the mission_control microcontroller when "Send" is pressed.

		@param self  - Instance of the class.
		@param *args - Any other random system varaible that gets passed in.
		"""

		temp_packet = ""

		# Update 
		temp_packet += "$"
		temp_packet += ","
		
		temp_packet += str(self.operational_mode.get())
		temp_packet += ","
		temp_packet += str(self.roll_call_status.get())
		temp_packet += ","
		
		

		self.modified_commands.set(temp_packet)

	def callback_roll_call_start(self):
		"""
		Triggered by the press of "button_roll_call_start".

		@param self - Instance of the class.
		"""

		self.roll_call_status.set("RUNNING")
		self.operational_mode.set("ROLL CALL")

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
				# If non-null, send transmission via serial port.
				send(g.PORT_MC_LORA.get_port(), converted_transmission)
				print("Completed connection to lora.")
		# Null connection.
		except Exception as e:
			# Prints general error statement. (Used to tell which method errored out)
			print("Invalid connection to mission control's lora.")
			print("Exception: " + str(e))
			
	def callback_update(self):
		"""
		Method is run each time the connected microcontrollers send serial data to the gui.
		This method is triggered via a .trace() on the StringVar object .input which
		can be found near the bottom of the communication.py file.

		@param self - Instance of the class.
		@param *args - Any other random system varaible that gets passed in.
		"""

		temp_input = ""
###
		###
		###
		###
		###
		###
		###
		###
		###
		###
		###
		###
		###
		self.current_throttle.set("0");
		# Checks for none null connection to mission_control Uc.
		if g.PORT_MC_LORA is not None:
			temp_input = g.PORT_MC_LORA.input.get()
			# N signifies the packet being of normal communication type.
			if "N" in temp_input:
				serial_data, radio_data = str(temp_input).split("]")
				# Variables such as '$' and 'N' are thrown out as junk.
				# t_ stands for temp because the numbers need to be converted to the 
				# corresponding string for the gui to show.
				junk, junk ,t_craft_ts, t_alt, t_lat, t_lon, t_event, t_craft_id, t_mc_ts, t_c_sp, t_t_d, t_sat_c = str(serial_data).split(",")
				t_radio_in, t_radio_out, junk = str(radio_data).split("/")
				# Setting individual variables from the parsed packet.
				print(t_alt)
				'''self.home_time.set(t_mc_ts)
				self.craft_altitude.set(t_alt)
				self.craft_latitude.set(t_lat)
				self.craft_longitude.set(t_lon)
				self.craft_event.set(t_event)
				self.craft_time.set(t_craft_ts)
				self.craft_received_id.set(t_craft_id)
				self.radio_received.set(t_radio_in)
				self.radio_sent.set(t_radio_out)'''
				#self.current_speed.set(t_c_sp)
				#self.target_distance.set(t_t_d)
				#self.satillite_count.set(t_sat_c)
			# R signifies the packet being of type Roll Call.
			elif "R" in temp_input:
				# Variables such as '$' and 'R' are thrown out as junk.
				# t_ stands for temp because the numbers need to be converted to the 
				# corresponding string for the gui to show.
				junk, junk, t_mc_node, t_ee_node, t_relay_node, junk = str(temp_input).split(",")
				# Setting individual variables from the parsed packet.
				'''self.node_mission_control.set(t_mc_node)
				self.node_eagle_eye.set(t_ee_node)
				self.node_relay.set(t_relay_node)'''

		if g.PORT_CRAFT_LORA is not None:
			placeholder = 1 + 1

		if g.PORT_CRAFT_MEGA is not None:
			placeholder = 1 + 1



		


	def convert_serial(self):
		"""
		Responsible for taking the variables to be set via serial and converting 
		them to their correct integer value.

		@param self - Instance of the class.
		"""

		new_packet = ""

		# Checks for normal serial operations.
		if self.authority_mode is "AUTO":
			new_packet += "$"
			new_packet += ","
			new_packet += convert_authority()
			new_packet += ","
			new_packet += convert_op_mode()
			new_packet += ","
			new_packet += convert_rc_mode()
			new_packet += ","
			new_packet += convert_anchor()
			new_packet += ","
			new_packet += self.target_throttle
			new_packet += ","
			new_packet += self.target_altitude
			new_packet += ","
			new_packet += self.target_latitude
			new_packet += ","
			new_packet += self.target_longitude
			new_packet += ","
			new_packet += "$"

		# Checks for roll call serial operations.
		elif self.authority_mode is "MANUAL":
			new_packet += "$"
			new_packet += ","
			new_packet += convert_authority()
			new_packet += ","
			new_packet += convert_direction()
			new_packet += ","
			new_packet += convert_anchor()
			new_packet += ","
			new_packet += "$"

	
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
