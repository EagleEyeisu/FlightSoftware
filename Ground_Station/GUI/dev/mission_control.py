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
import globals as g
import maps as maps
import time
import threading
import urllib.request


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
		self.node_mission_control = None
		self.node_craft = None
		self.radio_received = None
		self.radio_sent = None
		self.radio_rssi = None
		self.radio_last_contact = None

		# Eagle Eye Craft variables.
		self.craft_time = None
		self.craft_altitude = None
		self.craft_latitude = None
		self.craft_longitude = None
		self.craft_event = None
		self.craft_speed = None
		self.craft_roll = None
		self.craft_pitch = None
		self.craft_yaw = None
		self.craft_state = None
		self.craft_map_image = None

		# Mission Control variables.
		self.mission_control_time = None
		self.manual_direction = None
		self.craft_anchor = None
		self.target_throttle = None
		self.craft_time_previous = ""
		self.network_map_image = None


	def variable_setup(self):
		"""
		Initializes classe variables to proper types and starting values.
		@param self - Instance of the class.
		"""

		# StringVar variables are strings that update their text in entries when
		# their value is changed.
		self.mission_control_time = StringVar(self.mc_frame)
		self.node_mission_control = StringVar(self.mc_frame)
		self.manual_direction = StringVar(self.mc_frame)
		self.craft_anchor = StringVar(self.mc_frame)
		self.target_throttle = StringVar(self.mc_frame)
		self.node_craft = StringVar(self.mc_frame)
		self.radio_received = StringVar(self.mc_frame)
		self.radio_sent = StringVar(self.mc_frame)
		self.radio_rssi = StringVar(self.mc_frame)
		self.radio_last_contact = StringVar(self.mc_frame)
		self.craft_time = StringVar(self.mc_frame)
		self.craft_altitude = StringVar(self.mc_frame)
		self.craft_latitude = StringVar(self.mc_frame)
		self.craft_longitude = StringVar(self.mc_frame)
		self.craft_event = StringVar(self.mc_frame)
		self.craft_speed = StringVar(self.mc_frame)
		self.craft_roll = StringVar(self.mc_frame)
		self.craft_pitch = StringVar(self.mc_frame)
		self.craft_yaw = StringVar(self.mc_frame)
		self.craft_state = StringVar(self.mc_frame)

		# Configures tracing for all variables. When these variables are written to,
		# the corresponding method will run. (Allows for real time display updating)
		self.node_mission_control.trace("w", self.callback_update_mission_control_node_status)
		self.node_craft.trace("w", self.callback_update_craft_node_status)

		# Initialization of varaible values on GUI startup.
		self.radio_received.set("-------")
		self.radio_sent.set("-------")
		self.radio_rssi.set("-------")
		self.radio_last_contact.set("-------")
		self.craft_time.set("-------")
		self.craft_altitude.set("-------")
		self.craft_latitude.set("-------")
		self.craft_longitude.set("-------")
		self.craft_event.set("-------")
		self.craft_speed.set("-------")
		self.craft_roll.set("-------")
		self.craft_pitch.set("-------")
		self.craft_yaw.set("-------")
		self.craft_state.set("-------")
		self.mission_control_time.set("-------")
		self.manual_direction.set("-------")
		self.craft_anchor.set("-------")
		self.target_throttle.set("-------")


	def create_entry_objects(self):
		"""
		Creates/configures entry objects.
		@param self - Instance of the class.
		"""

		# Creates entry widgets for each variable. (Text windows to be placed in the GUI)
		self.entry_radio_received = Entry(self.mc_frame, state="readonly", textvariable=self.radio_received, font='Helvtica 11')
		self.entry_radio_sent = Entry(self.mc_frame, state="readonly", textvariable=self.radio_sent, font='Helvtica 11')
		self.entry_radio_rssi = Entry(self.mc_frame, state="readonly", textvariable=self.radio_rssi, font='Helvtica 11')
		self.entry_radio_last_contact = Entry(self.mc_frame, state="readonly", textvariable=self.radio_last_contact, font='Helvtica 11')
		self.entry_craft_time = Entry(self.mc_frame, state="readonly", textvariable=self.craft_time, justify='right', font='Helvtica 11')
		self.entry_craft_altitude = Entry(self.mc_frame, state="readonly", textvariable=self.craft_altitude, justify='right', font='Helvtica 11')
		self.entry_craft_latitude = Entry(self.mc_frame, state="readonly", textvariable=self.craft_latitude, justify='right', font='Helvtica 11')
		self.entry_craft_longitude = Entry(self.mc_frame, state="readonly", textvariable=self.craft_longitude, justify='right', font='Helvtica 11')
		self.entry_craft_event = Entry(self.mc_frame, state="readonly", textvariable=self.craft_event, justify='right', font='Helvtica 11')
		self.entry_craft_speed = Entry(self.mc_frame, state="readonly", textvariable=self.craft_speed, justify='right', font='Helvtica 11')
		self.entry_mission_control_time = Entry(self.mc_frame, state="readonly", textvariable=self.mission_control_time, justify='right', font='Helvtica 11')
		self.entry_manual_direction = Entry(self.mc_frame, state="readonly", textvariable=self.manual_direction, justify='right', font='Helvtica 11')
		self.entry_craft_anchor = Entry(self.mc_frame, state="readonly", textvariable=self.craft_anchor, justify='right', font='Helvtica 11')
		self.entry_target_throttle = Entry(self.mc_frame, state="readonly", textvariable=self.target_throttle, justify='right', font='Helvtica 11')


	def create_button_objects(self):
		"""
		Creates/configures button objects.
		@param self - Instance of the class.
		"""

		# Creates button widgets. (Triggers specified callback method.)
		self.button_craft_zoom_in = Button(self.mc_frame, text="Zoom In", command=self.callback_craft_zoom_in)
		self.button_craft_zoom_out = Button(self.mc_frame, text="Zoom Out", command=self.callback_craft_zoom_out)
		self.button_craft_map_type = Button(self.mc_frame, text="Road/Hybrid", command=self.callback_craft_maptype)
		self.button_network_zoom_in = Button(self.mc_frame, text="Zoom In", command=self.callback_network_zoom_in)
		self.button_network_zoom_out = Button(self.mc_frame, text="Zoom Out", command=self.callback_network_zoom_out)
		self.button_network_map_type = Button(self.mc_frame, text="Road/Hybrid", command=self.callback_network_maptype)
		self.button_manual_forward = Button(self.mc_frame, text="FORWARD", command=self.callback_manual_forward)
		self.button_manual_backward = Button(self.mc_frame, text="BACKWARD", command=self.callback_manual_backward)
		self.button_manual_left = Button(self.mc_frame, text="LEFT", command=self.callback_manual_left)
		self.button_manual_right = Button(self.mc_frame, text="RIGHT", command=self.callback_manual_right)
		self.button_manual_stop = Button(self.mc_frame, text="STOP", command=self.callback_manual_stop)


	def create_label_objects(self):
		"""
		Creates/configures Label objects.
		@param self - Instance of the class.
		"""

		# These objects are banners that are used to give context to each corresponding section
		# of either buttons, display bars, or other objects on the GUI.
		self.label_mission_control_node = Label(self.mc_frame, text="GROUND STATION", relief='solid', anchor="center")
		self.label_mission_control_node.configure(background='red')
		self.label_craft_node = Label(self.mc_frame, text="CRAFT", relief='solid', anchor="center")
		self.label_craft_node.configure(background='red')


	def layout_network(self):
		"""
		Binds the sections of widgets related to the network to the top
		portion of the frame.
		@param self - Instance of the class.
		"""

		# Above divider one. (divider at bottom of method)
		self.create_label_east_2(1, 0, self.mc_frame, "Node Status:")
		self.label_mission_control_node.grid(row=0, column=1, rowspan=2, sticky='nswe')
		self.label_craft_node.grid(row=2, column=1, rowspan=2, sticky='nswe')
		self.create_label_east(0, 5, self.mc_frame, "Received:")
		self.entry_radio_received.grid(row=0, column=6, columnspan=8, sticky='we')
		self.create_label_east(1, 5, self.mc_frame, "Sent:")
		self.entry_radio_sent.grid(row=1, column=6, columnspan=8, sticky='we')
		self.create_label_east(2, 5, self.mc_frame, "RSSI Craft:")
		self.entry_radio_rssi.grid(row=2, column=6, sticky='we')
		self.create_label_east(2, 8, self.mc_frame, "Last Contact (s):")
		self.entry_radio_last_contact.grid(row=2, column=9, sticky='w')

		# Terminal divider. KEEP AT THE BOTTOM OF THIS METHOD.
		# This divider is a golden bar strecthing across the screen to provide
		# distinction between variable sections.
		terminal_divider_one = Label(self.mc_frame, background="#F1BE48")
		terminal_divider_one.grid(row=5, column=0, columnspan=14, sticky='we')


	def layout_nodes(self):
		"""
		Binds the sections of widgets related to the craft to the middle
		portion of the frame.
		@param self - Instance of the class.
		"""

		# PAYLOAD
		self.create_label_center(6, 1, self.mc_frame, "CRAFT")
		self.create_label_center(7, 0, self.mc_frame, "Up Time (s): ")
		self.entry_craft_time.grid(row=7, column=1, columnspan=2, sticky='we')
		self.create_label_center(8, 0, self.mc_frame, "Altitude (m): ")
		self.entry_craft_altitude.grid(row=8, column=1, columnspan=2, sticky='we')
		self.create_label_center(9, 0, self.mc_frame, "Latitude:       ")
		self.entry_craft_latitude.grid(row=9, column=1, columnspan=2, sticky='we')
		self.create_label_center(10, 0, self.mc_frame, "Longitude:   ")
		self.entry_craft_longitude.grid(row=10, column=1, columnspan=2, sticky='we')
		self.create_label_center(11, 0, self.mc_frame, "Speed:            ")
		self.entry_craft_speed.grid(row=11, column=1, columnspan=2, sticky='we')
		#self.create_label_center(11, 0, self.mc_frame, "Speed:            ")
		#self.entry_craft_speed.grid(row=11, column=1, columnspan=2, sticky='we')

		# MISSION CONTROL
		self.create_label_center(11, 11, self.mc_frame, "GROUND")
		self.create_label_center(12, 10, self.mc_frame, "Up Time (s):   ")
		self.entry_mission_control_time.grid(row=12, column=11, columnspan=2, sticky='we')


	def layout_mission_maps(self):
		"""
		Binds placeholder maps to the GUI. These will later be updated once GPS data
		starts to feed in from various nodes.

		@param self - Instance of the class.
		"""

		# Pulls Static Maps image into TKinter GUI. (Placeholder)
		temp_image = PhotoImage(file="gui_maps/offline.png")
		
		# Binds image inside of label object. (Needed to use the grid layout)
		self.craft_map_image = Label(self.mc_frame, image=temp_image)
		# Reassigns the label object with the image attribute.
		self.craft_map_image.image = temp_image
		# Places image into GUI.
		self.craft_map_image.grid(row=17, column=0, columnspan=4, rowspan=4, sticky='nswe')
		self.button_craft_zoom_in.grid(row=16, column=3, sticky="nsew")
		self.button_craft_zoom_out.grid(row=16, column=0, sticky="nsew")
		self.button_craft_map_type.grid(row=16, column=1, columnspan=2)
		self.create_label_center_2(15, 1, self.mc_frame, "CRAFT")

		# Binds image inside of label object. (Needed to use the grid layout)
		self.network_map_image = Label(self.mc_frame, image=temp_image)
		# Reassigns the label object with the image attribute.
		self.network_map_image.image = temp_image
		# Places image into GUI.
		self.network_map_image.grid(row=17, column=10, columnspan=4, rowspan=4, sticky='nswe')
		self.button_network_zoom_in.grid(row=16, column=13, sticky="nsew")
		self.button_network_zoom_out.grid(row=16, column=10, sticky="nsew")
		self.button_network_map_type.grid(row=16, column=11, columnspan=2)
		self.create_label_center_2(15, 11, self.mc_frame, "ALL NODES")

	
	def layout_manual_controls(self):
		"""
		Lays out the buttons for the user to command the craft.
		@param self - Instance of the Class.
		"""

		# Craft
		self.button_manual_forward.grid(row=18, column=6, sticky="nsew")
		self.button_manual_backward.grid(row=20, column=6, sticky="nsew")
		self.button_manual_stop.grid(row=19, column=6, sticky="nsew")
		self.button_manual_left.grid(row=19, column=5, sticky="nsew")
		self.button_manual_right.grid(row=19, column=7, sticky="nsew")


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
		self.create_label_objects()
		# Aligns the widgets to the frame's grid.
		self.layout_network()
		self.layout_nodes()
		self.layout_mission_maps()
		self.layout_manual_controls()
		# Update class instance stored as global.
		g.mc_class_reference = self


	def callback_manual_stop(self):
		self.manual_direction.set("STOP")
		self.send_to_ground_station()


	def callback_manual_forward(self):
		print("In callback forward")
		self.manual_direction.set("FORWARD")
		self.send_to_ground_station()
		print("After sent")


	def callback_manual_backward(self):
		self.manual_direction.set("BACKWARD")
		self.send_to_ground_station()


	def callback_manual_left(self):
		self.manual_direction.set("LEFT")
		self.send_to_ground_station()


	def callback_manual_right(self):
		self.manual_direction.set("RIGHT")
		self.send_to_ground_station()


	def callback_craft_zoom_in(self, *args):
		"""
		Increases the zoom level of the craft map image. Also requests
		& replaces the current map image.

		@param self - Instace of the class.
		"""

		# Increase the zoom level.
		maps.craft_zoom = str(int(maps.craft_zoom) + 1)
		# Constructs the url needed to get the right image from Static Maps.
		url = maps.build_url_craft(maps.previous_craft_coords)
		# Directory information and file name.
		image_name = "gui_maps/craft_map" + "." + "PNG"
		# Pulls down the configured Static Maps image from Google.
		maps.request_API_image(url, image_name)
		if self.craft_map_image is not None:
			self.craft_map_image.destroy()
		# Places the new image into the GUI.
		self.mc_frame = maps.place_craft(self.mc_frame)


	def callback_craft_zoom_out(self, *args):
		"""
		Decreases the zoom level of the craft map image. Also requests
		& replaces the current map image.

		@param self - Instace of the class.
		"""

		# Decreases the zoom level.
		maps.craft_zoom = str(int(maps.craft_zoom) - 1)
		# Constructs the url needed to get the right image from Static Maps.
		url = maps.build_url_craft(maps.previous_craft_coords)
		# Directory information and file name.
		image_name = "gui_maps/craft_map" + "." + "PNG"
		# Pulls down the configured Static Maps image from Google.
		maps.request_API_image(url, image_name)
		if self.craft_map_image is not None:
			self.craft_map_image.destroy()
		# Places the new image into the GUI.
		self.mc_frame = maps.place_craft(self.mc_frame)


	def callback_craft_maptype(self, *args):
		"""
		Toggles the maptype of the requested image between "roadmap"
		& "hybrid" (satellite view w/ roads).

		@param self - Instace of the class.
		"""
	
		if maps.craft_maptype is "roadmap":
			maps.craft_maptype = "hybrid"
		else:
			maps.craft_maptype = "roadmap"


	def callback_network_zoom_in(self, *args):
		"""
		Increases the zoom level of the network map image. Also requests
		& replaces the current map image.

		@param self - Instace of the class.
		"""

		# Increase the zoom level.
		maps.network_zoom = str(int(maps.network_zoom) + 1)
		# Constructs the url needed to get the right image from Static Maps.
		url = maps.build_url_network()
		# Directory information and file name.
		image_name = "gui_maps/network_map" + "." + "PNG"
		# Pulls down the configured Static Maps image from Google.
		maps.request_API_image(url, image_name)
		# Places the new image into the GUI.
		if self.network_map_image is not None:
			self.network_map_image.destroy()
		# Places the new image into the GUI.
		self.mc_frame = maps.place_network(self.mc_frame)


	def callback_network_zoom_out(self, *args):
		"""
		Decreases the zoom level of the network map image. Also requests
		& replaces the current map image.

		@param self - Instace of the class.
		"""

		# Decreases the zoom level.
		maps.network_zoom = str(int(maps.network_zoom) - 1)
		# Constructs the url needed to get the right image from Static Maps.
		url = maps.build_url_network()
		# Directory information and file name.
		image_name = "gui_maps/network_map" + "." + "PNG"
		# Pulls down the configured Static Maps image from Google.
		maps.request_API_image(url, image_name)
		# Places the new image into the GUI.
		if self.network_map_image is not None:
			self.network_map_image.destroy()
		# Places the new image into the GUI.
		self.mc_frame = maps.place_network(self.mc_frame)


	def callback_network_maptype(self, *args):
		"""
		Toggles the maptype of the requested image between "roadmap"
		& "hybrid" (satellite view w/ roads).

		@param self - Instace of the class.
		"""
	
		if maps.network_maptype is "roadmap":
			maps.network_maptype = "hybrid"
		else:
			maps.network_maptype = "roadmap"


	def callback_update_mission_control_node_status(self, *args):
		"""
		Upon serial data notification that the mc_node's network status has been
		updated, this method will change the color of the visual representation on
		the gui to inform the user.
		Green = Connected.
		Yellow = Was, but lost.
		Red = Not connected / lost.
		@param self - Instance of the class.
		"""

		# Refer to above documentation for what the numbers mean.
		if self.node_mission_control.get() in "0.00":
			self.label_mission_control_node.configure(background='red')
		elif self.node_mission_control.get() in "1.00":
			self.label_mission_control_node.configure(background='green')
		elif self.node_mission_control.get() in "2.00":
			self.label_mission_control_node.configure(background='yellow')


	def callback_update_craft_node_status(self, *args):
		"""
		Upon serial data notification that the craft_node's network status has been
		updated, this method will change the color of the visual representation on
		the gui to inform the user.
		Green = Connected.
		Yellow = Was, but lost.
		Red = Not connected / lost.
		@param self - Instance of the class.
		"""

		# Refer to above documentation for what the numbers mean.
		if self.node_craft.get() in "0.00":
			self.label_craft_node.configure(background='red')
		elif self.node_craft.get() in "1.00":
			self.label_craft_node.configure(background='green')
		elif self.node_craft.get() in "2.00":
			self.label_craft_node.configure(background='yellow')


	def callback_update_gui(self, *args):
		"""
		Method is run each time the connected microcontrollers send serial data to the gui.
		This method is triggered via a .trace() on the StringVar object .input which
		can be found near the bottom of the communication.py file.
		@param self - Instance of the class.
		@param *args - Any other random system varaible that gets passed in.
		"""

		# Ensure the memory is clear.
		temp_input = ""
		# Checks for a none null connection to mission_control microcontroller.
		if g.PORT_MISSION_CONTROL_RADIO is not None:
			# If valid connection, get its serial data input.
			temp_input = g.PORT_MISSION_CONTROL_RADIO.input.get()
			self.node_mission_control.set("1")
			if(temp_input.count("/") is 5):
				# Splits the received serial data into two respective parts.
				junk, local_vars, radio_in, radio_out, received_rssi, junk = str(temp_input).split("/")
				self.parse_mission_control(local_vars)
				self.parse_craft(radio_in, received_rssi)
				self.radio_sent.set(radio_out)
				self.radio_received.set(radio_in)
				# Prints input.
				#print("---------------------------------------------------------------------------------------------")
				#print("Input: " + str(temp_input))
				#print("Local: " + str(local_vars))
				#print("Pkt Received: " + str(radio_in))
				#print("Pkt Sent: " + str(radio_out))
				#print("RSSI: " + str(received_rssi))
				# SD Card.
				self.log_file = open('mc_data.txt','a') 
				self.log_file.write("---------------------------------------------------------------------------------------------" +"\n")
				self.log_file.write("Input: " + str(temp_input) +"\n")
				self.log_file.write("Local: " + str(local_vars) +"\n")
				self.log_file.write("Pkt Received: " + str(radio_in) +"\n")
				self.log_file.write("Pkt Sent: " + str(radio_out) +"\n")
				self.log_file.write("RSSI: " + str(received_rssi) +"\n")
				self.log_file.close()


	def parse_mission_control(self, local_vars):
		"""
		Parses out variables from given string section.
		Assigns to correct variables.
		
		@param self - Instance of the class.
		@param local_vars - Variables associated with the connected microcontroller.
		"""
		
		# Parses out the mission control timestamp.
		mc_ts = local_vars
		# Sets parsed values to their corresponding StringVar.
		self.mission_control_time.set(mc_ts)


	def parse_craft(self, radio_in, rssi):
		"""
		Parses out variables from given string section.
		Assigns to correct variables.
		
		@param self - Instance of the class.
		@param radio_in - Packet received via radio.
		@param rssi - Relative Signal Strength Index for the received packet.
		"""
		
		# Checks for radio in data. Parses if valid.
		try:
			# Checksums '$' and non craft variables are thrown out.
			junk, c_ts, c_roll, c_pitch, c_yaw, c_state, j_ts, j_md, junk = str(radio_in).split(",")
		# No Radio data yet.
		except Exception as e:
			# Skip rest of method.
			return
		# Sets parsed values to their corresponding StringVar.
		self.craft_time.set(c_ts)
		self.craft_roll.set(c_roll)
		self.craft_pitch.set(c_pitch)
		self.craft_yaw.set(c_yaw)
		self.craft_state.set(c_state)
		# Only reads in a new packet.
		if self.craft_time_previous != str(self.craft_time.get()):
			self.node_craft.set("1")
			# Updates the appropriate variables.
			self.craft_time_previous = str(self.craft_time.get())
			self.update_craft_rssi(rssi)
		# Configure a map image from Google Static Maps API.
		#new_map_flag = maps.generate_map(self.craft_latitude.get(), self.craft_longitude.get())
		# If a new map was created (new GPS coords), places that image into the GUI.
		# maps.
		#if new_map_flag is True:
			# Reset flag.
			#new_map_flag = False
			#maps.place_craft(self.mc_frame)
			#maps.place_network(self.mc_frame)
			

	def update_craft_rssi(self, received_rssi):
		"""
		Updates GUI's variables holding information about the last received
		craft packet.
		@param self  - Instance of the class.
		@param *args - The RSSI of the last received packet.
		"""

		# Checks if timer is already running.
		if g.timer_contact is not None:
			# If so, disable it to resync the 1sec timer.
			g.timer_contact_.cancel()
		# If so, assign RSSI to the craft variables.
		self.radio_rssi.set(received_rssi)
		# Reset the last contact timer.
		self.radio_last_contact.set("0")
		# Creates countdown timer that, upon hitting zero runs the associated method.
		# Units are seconds.
		g.timer_contact_ = threading.Timer(1.0, self.timer_increment_last_contact)
		# Starts the countdown timer.
		g.timer_contact_.start()


	def timer_increment_last_contact(self):
		"""
		Increments the craft node's last contact variable each second.
		This timer is resynced each time a packet from the craft
		is received.
		@param self  - Instance of the class.
		"""

		# Checks if timer is already running.
		if g.timer_contact is not None:
			# If so, disable it to resync the 1sec timer.
			g.timer_contact_.cancel()
		# Increments the craft last contact timer on a 1 second interval.
		self.radio_last_contact.set(str(int(self.radio_last_contact.get()) + 1))
		# Creates countdown timer that, upon hitting zero runs the associated method.
		# Units are seconds.
		g.timer_contact = threading.Timer(1.0, self.timer_increment_last_contact)
		# Starts the countdown timer.
		g.timer_contact.start()


	def send_to_ground_station(self):
		"""
		Triggered by the press of a direction button. (Forward, backward, etc..)

		@param self         - Instance of the class.
		"""

		try:
			# Checks for non-null connection to mission control's lora microcontroller.
			if g.PORT_MISSION_CONTROL_RADIO is not None:
				# If non-null, send transmission via serial port.
				send(g.PORT_MISSION_CONTROL_RADIO.get_port(), self.create_injection_packet())
		# Null connection.
		except Exception as e:
			# Prints general error statement. (Used to tell which method errored out)
			print("Invalid connection to mission control's uC.")
			print("Exception: " + str(e))


	def create_injection_packet(self):
		"""
		Creates a manually specified packet to be sent to the connected LoRa node.
		@param self - Instance of the class.
		"""

		new_packet = ""

		try:
			new_packet += "$"
			new_packet += ","
			new_packet += str(self.convert_direction())
			new_packet += ","
			new_packet += "$"
			# Returns new packet.
			return new_packet

		# Prints exception handler.
		except Exception as e:
			# Prints general error statement. (Used to tell which method errored out)
			print("Unable to create packet.")
			print("Exception: " + str(e))


	def convert_direction(self):
		"""
		Converts to correct integer value.

		@param self - Instance of the class.
		"""

		if self.manual_direction.get() == "STOP":
			return "0.00"
		elif self.manual_direction.get() == "FORWARD":
			return "1.00"
		elif self.manual_direction.get() == "BACKWARD":
			return "2.00"
		elif self.manual_direction.get() == "LEFT":
			return "3.00"
		elif self.manual_direction.get() == "RIGHT":
			return "4.00"


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

	
	def create_label_east_2(self, r, c, frame, title):
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
		label.grid(row=r, column=c, rowspan=2, sticky='ens')


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


	def create_label_center_2(self, r, c, frame, title):
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
		label.grid(row=r, column=c, columnspan=2)
