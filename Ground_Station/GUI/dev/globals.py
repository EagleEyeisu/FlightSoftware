#############################################################
#
#	Property of Eagle Eye.
#
#   Authors:
#           Jared Danner
#
#############################################################

# System user id.
SYSTEM_USER = None

# Serial port objects. Object class defined at bottom of communication.py.
# If let None, it will dynamically find its correct port #.
PORT_MISSION_CONTROL_RADIO = None
PORT_CRAFT_RADIO = None
PORT_ROTOR_CONTROLLER = None

# Node ID's.
NODE_MISSION_CONTROL_ID = 0
NODE_CRAFT_ID = 1

# Tkinter frame objects.
mc_class_reference = None
craft_class_reference = None

# Threaded timer objects.
timer_mission_control_radio = None
timer_craft_radio = None
timer_mission_control_contact_timer = None
timer_craft_contact_timer = None