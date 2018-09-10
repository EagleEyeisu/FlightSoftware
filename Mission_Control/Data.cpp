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
	// General delay to ensure the serial port is not trying to both read and write.
	delay(50);

	// Send useful information back to the python GUI.
	Data.update_gui();
}


/**
 * Interrupt set to the serial port going high. 
 */
void serialEvent()
{

	String temp_input = "";
	while(Serial.available())
	{
		char t = Serial.read();
		temp_input += t;
	}

	char toParse[temp_input.length()];
	temp_input.toCharArray(toParse,temp_input.length());

	// Checks for correct data format.
	if(toParse[0]=='$')
	{
		// '0' at index 3 signifies manual control. 
		if(toParse[2]=='0')
		{
			Radio.Network.authority_mode = Data.get_serial_authority_mode(toParse);
			Radio.Network.target_direction = Data.get_serial_direction(toParse);
			Radio.Network.target_throttle = Data.get_serial_target_throttle(toParse);
			Radio.Network.craft_anchor = Data.get_serial_craft_anchor(toParse);
		}
		// '1' at index 3 signifies automatic control. 
		else if(toParse[2]=='1')
		{
			// To be implemented at a later date.
		}
	}
}


/**
 * Sends mission_control & craft inforamtion back to the GUI.
 */
void DATA::update_gui()
{
	if(!Serial.available())
	{
		String temp_packet = "";
		confirmed_packet = false;

		// Roll Call config.
		if(Radio.operation_mode == "ROLLCALL")
		{	
			confirmed_packet = true;
			temp_packet += "$";
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
		else if((Radio.operation_mode == "NORMAL") || (Radio.operation_mode == "STANDBY") || (Radio.operation_mode == "NONE"))
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
			temp_packet += "/";
			temp_packet += Radio.radio_input;
			temp_packet += "/";
			temp_packet += Radio.radio_output;
			temp_packet += "/";
			temp_packet += "$";
		}

		if(confirmed_packet){
			// Converts from String to char array. 
			char serial_packet[temp_packet.length()];
			temp.toCharArray(serial_packet, temp_packet.length());
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



