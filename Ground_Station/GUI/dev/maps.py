#############################################################
#
#	Property of Eagle Eye.
#
#   Authors:
#           Jared Danner
#
#############################################################

import urllib.request
from tkinter import *
from tkinter.ttk import *


google_api_key = "&key=AIzaSyBPC7y0fIjsbz61P16wTudIrkfKK0Unzd4"
url_base = "https://maps.googleapis.com/maps/api/staticmap?center="
zoom_base = "&zoom="
size_base = "&size="
maptype_base = "&maptype="
marker_base = "&markers="
howe_hall_coords = "42.026695,-93.653384"
# Used to compare previous map's coordinate to new coordinates to prevent
# requesting the exact same map from Google's API.
previous_craft_coords = "42.026695,-93.653384"
previous_recovery_coords = "42.026695,-93.653384"
# Toggled by buttons associated with the image objects.
craft_zoom = "10"
craft_maptype = "roadmap"
recovery_zoom = "10"
recovery_maptype = "roadmap"
network_zoom = "10"
network_maptype = "roadmap"


def generate_map(latitude, longitude, node):
    """
    Oversees the process of interacting with Google Maps API, populating 
    a requested map image with the lat & lng provided, and returning a static image
    to be placed in the Tkinter GUI.
    
    @param latitude - Latitude value to mark on the map image.
    @param longitude - Longitude value to mark on the map image.
    @param node - Craft or Recovery
    """

    # Combines the given lat & lng.
    map_center = str(latitude) + "," + str(longitude)    
    
    # Check for craft.
    if "craft" in node:
        # Check if the "new" coordinates are the same as last time.
        # Prevents duplicate map calls from the API.
        if map_center is not previous_craft_coords:
            # Update previous value.
            previous_craft_coords = map_center
            # Configures the Url needed to get the correct map from Google.
            url = build_url_craft(map_center)
            image_name = "gui_maps/craft_map" + "." + "PNG"
            # Pulls down the configured Static Maps image from Google.
            request_API_image(url, image_name)
            # Configures the big picture map. (containing all nodes in 1 map).
            url = build_url_network()
            image_name = "gui_maps/network_map" + "." + "PNG"
            # Pulls down the configured Static Maps image from Google.
            request_API_image(url, image_name)
            # New map has been generated. Replace the current image.
            return True
        # A map with this configuration has already been generated.
        # Return false to tell the software to not replace the current image.
        else:
            return False
    # Check for recovery.
    elif "recovery" in node:
        # Check if the "new" coordinates are the same as last time.
        # Prevents duplicate map calls from the API.
        if map_center is not previous_recovery_coords:
            # Update previous value.
            previous_recovery_coords = map_center
            # Configures the Url needed to get the correct map from Google.
            url = build_url_recovery(map_center)
            image_name = "gui_maps/recovery_map" + "." + "PNG"
            # Pulls down the configured Static Maps image from Google.
            request_API_image(url, image_name)
            # Configures the big picture map. (containing all nodes in 1 map).
            url = build_url_network()
            image_name = "gui_maps/network_map" + "." + "PNG"
            # Pulls down the configured Static Maps image from Google.
            request_API_image(url, image_name)
            # New map has been generated. Replace the current image.
            return True
        # A map with this configuration has already been generated.
        # Return false to tell the software to not replace the current image.
        else:
            return False


def request_API_image(url, image_name):
    """
    This one requests an image. Pretty self explanatory.
    
    @param url - Constructed url to send to Google's Static Maps.
    @param image_name - Name (including file type) to save the image as.
    """

    urllib.request.urlretrieve(url, image_name)


def build_url_craft(map_center):
    """ 
    Builds the required url to request the desired image from Google.
    
    @param map_cetner - Center of the map (gps coordinates)
    """

    # Builds the url to request a specific image.
    map_url = None
    map_url = url_base + map_center
    map_url +=  zoom_base + craft_zoom
    map_url +=  size_base + "500x500"
    map_url +=  maptype_base + craft_maptype
    map_url +=  marker_base + "color:red%7Clabel:P%7C" + map_center
    map_url +=  google_api_key
    # Returns url.
    return map_url


def build_url_recovery(map_center):
    """ 
    Builds the required url to request the desired image from Google.
    
    @param map_cetner - Center of the map (gps coordinates)
    """

    # Builds the url to request a specific image.
    map_url = None
    map_url = url_base + map_center
    map_url +=  zoom_base + recovery_zoom
    map_url +=  size_base + "500x500"
    map_url +=  maptype_base + recovery_maptype
    map_url +=  marker_base + "color:green%7Clabel:R%7C" + map_center
    map_url +=  google_api_key
    # Returns url.
    return map_url


def build_url_network():
    """ 
    Builds the required url to request the desired image from Google.
    This image holds all nodes in the network.
    """

    # Seperates all latitude and longitude values from pairs into individual variables.
    p_lat, p_lng = previous_craft_coords.split(",")
    r_lat, r_lng = previous_recovery_coords.split(",")
    mc_lat, mc_lng = howe_hall_coords.split(",")

    # Calculates the center latitude value.
    center_lat = (float(p_lat) + float(r_lat) + float(mc_lat)) / 3.0
    center_lng = (float(p_lng) + float(r_lng) + float(mc_lng)) / 3.0

    map_center = str(center_lat) + "," + str(center_lng)

    # Builds the url to request a specific image.
    map_url = None
    map_url = url_base + map_center
    map_url +=  zoom_base + network_zoom
    map_url +=  size_base + "500x500"
    map_url +=  maptype_base + network_maptype
    map_url +=  marker_base + "color:green%7Clabel:R%7C" + map_center
    map_url +=  google_api_key
    # Returns url.
    return map_url
    

def place_craft(mc_frame):
    """
    Replaces the craft map w/ the updated one.

    @param mc_frame - Reference object that holds the craft_map_image object.
    """

    # Pulls Static Maps image into python.
    temp_image = PhotoImage(file="gui_maps/craft_map.png")
    # Binds image inside of label object. (Needed to use the grid layout)
    mc_frame.craft_map_image = Label(mc_frame, image=temp_image)
    # Reassigns the label object with the image attribute.
    mc_frame.craft_map_image.image = temp_image
    # Places image into GUI.
    mc_frame.craft_map_image.grid(row=15, column=0, rowspan=2, columnspan=4, sticky='nswe')
    # Returns the updated 
    return mc_frame


def place_recovery(mc_frame):
    """
    Replaces the recovery map w/ the updated one.

    @param mc_frame - Reference object that holds the recovery_map_image object.
    """

    # Pulls Static Maps image into python.
    temp_image = PhotoImage(file="gui_maps/recovery_map.png")
    # Binds image inside of label object. (Needed to use the grid layout)
    mc_frame.recovery_map_image = Label(mc_frame, image=temp_image)
    # Reassigns the label object with the image attribute.
    mc_frame.recovery_map_image.image = temp_image
    # Places image into GUI.
    mc_frame.recovery_map_image.grid(row=15, column=5, rowspan=2, columnspan=4, sticky='nswe')
    # Returns the updated 
    return mc_frame
    

def place_network(mc_frame):
    """
    Replaces the network map w/ the updated one.

    @param mc_frame - Reference object that holds the network_map_image object.
    """

    # Pulls Static Maps image into python.
    temp_image = PhotoImage(file="gui_maps/network_map.png")
    # Binds image inside of label object. (Needed to use the grid layout)
    mc_frame.network_map_image = Label(mc_frame, image=temp_image)
    # Reassigns the label object with the image attribute.
    mc_frame.network_map_image.image = temp_image
    # Places image into GUI.
    mc_frame.network_map_image.grid(row=15, column=10, rowspan=2, columnspan=4, sticky='nswe')
    # Returns the updated 
    return mc_frame