/**
 * Data.cpp contains the struct that holds all the Crafts situational information.
 */

#include <Arduino.h>
#include "Data.h"
#include "Radio.h"
#include <stdlib.h>
#include "Globals.h"

/**
 * Constructor used to reference all other variables & functions.
 */
DATA::DATA()
{
  
}


/**
 * Returns a parsed section of the read in parameter. The parameter 'objective' represents 
 * the comma's position from the beginning of the character array.
 */
float DATA::Parse(char message[], int objective)
{

	// Example GPS Transmission. (GGA)
	//
	// $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
	//
	// Example Radio Transmission. 
	//
	//                    LORA                                        MISSION CONTROL                       CRAFT ID
	// Time(ms),Altitude,Latitude,Longitude,LE, | Time(ms),craft_anchor,new_throttle,TargetLat,TargetLon, | Signal Origin
	//
	// The number of commas that the program needs to pass before it started parsing the data.

	// Used to iterate through the passed in character array.
	int i = 0;
	// This iterator is used to pull the wanted part of the 'message' from the entire array.
	// Used to gather information such as how long the new parsed section is.
	int tempIter = 0;
	// Counts the commas as the character array is iterated over.
	int commaCounter = 0;
	// This turns true when the correct number of commas has been achieved, which signals that the following 
	// section is the part that the program wants to parse from the entire sentence.
	bool Goal = false;
	// Temporary string used to hold the newly parsed array.
	char tempArr[20];
  	// Iterators over the entire array.
  	for(i=0;i<120;i++)
  	{
    	// Checks to see if the current iterator's position is a comma. 
    	if(message[i] == ',')
    	{
    		// If so, it iterators the comma counter by 1.
      		commaCounter++;
    	}
    	// Checks to see if the desired amount of commas has been passed. 
	    else if(commaCounter == objective)
	    {
		    // Checks to see if the iterator's position is a comma, used to cause a stop in parsing.
		    if(message[i] != ',')
		    {
		    	// Copies the message's character to the temporary array.
		        tempArr[tempIter] = message[i];
		        // Iterator used to tell how long the temporary array is.
		        tempIter++;
		    }
	    }
  	}
	// Charater array used with a fitted length of the parsed section.
	char arr[tempIter];
	// Iterates through the temporary array copying over the info to the variable which will be returned.
	for(i=0;i<tempIter;i++)
	{
		// Copying of the information between arrays.
	    arr[i]=tempArr[i];
	}
	// Converts the final array to a float.
	float temp = atof(arr);
	// Returns the desired parsed section in number (float) form.
  	return temp;
}


/**
 * Responsible for all serial communication between the GUI and mission_control microcontroller.
 */
void DATA::serial_comms()
{
	// Send useful information back to the python GUI.
	Data.update_gui();
}


/**
 * Sends mission_control & craft inforamtion back to the GUI.
 */
void DATA::update_gui()
{
  // Only sends info to update gui every 2 seconds to relieve traffic.
	if(!Serial.available() && (millis() - Data.serial_timer >= 2000))
	{
    // Resets / starts timer.
    Data.serial_timer = millis();
    // Holds outgoing message.
		String temp_packet = "";
    // Gets set true if a packet (TO BE SENT) has been created.
		bool confirmed_packet = false;
		// Roll Call config.
		if(Radio.operation_mode == Radio.ROLLCALL)
		{
			confirmed_packet = true;
			temp_packet += "$";
			temp_packet += ",";
      		temp_packet += "R";
      		temp_packet += ",";
			temp_packet += Radio.operation_mode;
			temp_packet += ",";
			temp_packet += Radio.mc_node.node_status;
			temp_packet += ",";
			temp_packet += Radio.ee_node.node_status;
			temp_packet += ",";
			temp_packet += Radio.relay_node.node_status;
			temp_packet += ",";
			temp_packet += "$";
		}
		// Normal GUI <-> mission_control Config.
		else if((Radio.operation_mode == Radio.NORMAL) || (Radio.operation_mode == Radio.STANDBY) || (Radio.operation_mode == Radio.NONE))
		{
			confirmed_packet = true;
			temp_packet += "$";
			temp_packet += ",";
			temp_packet += "N";
			temp_packet += ",";
			temp_packet += Radio.Network.craft_anchor;
			temp_packet += ",";
			temp_packet += Radio.Network.craft_altitude;
			temp_packet += ",";
			temp_packet += Radio.Network.craft_latitude;
			temp_packet += ",";
			temp_packet += Radio.Network.craft_longitude;
			temp_packet += ",";
			temp_packet += Radio.Network.craft_event;
			temp_packet += "]";
			temp_packet += Radio.radio_input;
			temp_packet += "/";
			temp_packet += Radio.radio_output;
			temp_packet += "/";
			temp_packet += "$";
      		temp_packet += "$"; // Weird serial issue where it only sends one of the '$'. To be looked into.
		}

		if(confirmed_packet){
			// Converts from String to char array. 
			char serial_packet[temp_packet.length()];
			temp_packet.toCharArray(serial_packet, temp_packet.length());
			// Sends packet via serial to python GUI.
			Serial.write(serial_packet);
		}
	}
}


/**
 * Parses serial input and returns the authority mode.
 */
float DATA::get_serial_authority_mode(char buf[])
{
    return (Parse(buf,1));
}


/**
 * Parses serial input and returns the user's manual direction.
 */
float DATA::get_serial_direction(char buf[])
{
    return (Parse(buf,2));
}


/**
 * Parses serial input and returns the anchor status.
 */
float DATA::get_serial_craft_anchor(char buf[])
{
    return (Parse(buf,3));
}


/**
 * Parses serial input and returns the target throttle.
 */
float DATA::get_serial_target_throttle(char buf[])
{
    return (Parse(buf,4));
}


/**
 * Parses serial input and returns the operational mode.
 */
void DATA::get_serial_op_mode(char buf[])
{	
	// Parses out operation_mode representated as integer.
    int temp_mode = (int)(Parse(buf,5));
    // Converts integer representation to the appropriate state.
    if(temp_mode == 0.0)
    {
    	Radio.operation_mode = Radio.NONE;
    }
    else if(temp_mode == 1.0)
    {
    	Radio.operation_mode = Radio.ROLLCALL;
    }
    else if(temp_mode == 2.0)
    {
    	Radio.operation_mode = Radio.STANDBY;
    }
    else if(temp_mode == 1.0)
    {
    	Radio.operation_mode = Radio.NORMAL;
    }
    else{ // Debug scenario.
      //Serial.write("nope");
    }
}
