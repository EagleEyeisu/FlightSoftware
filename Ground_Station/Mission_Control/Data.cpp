/**
 * Data.cpp contains the struct that holds all the Craft's situational information.
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
float DATA::Parse(int objective)
{
	// Used to iterate through the passed in character array.
	int iter = 0;
	// Counts the commas as the character array is iterated over.
	int comma_counter = 0;
	// Holds the desired data from the larger string.
	String wanted_data = "";
  	// Iterators over the entire array.
  	for(iter=0; iter<Radio.radio_input.length(); iter++)
  	{
    	// Checks to see if the current iterator's position is a comma.
    	if(Radio.radio_input[iter] == ',')
    	{
    		// If so, it iterators the comma counter by 1.
      		comma_counter++;
    	}
    	// Checks to see if the desired amount of commas has been passed.
	    else if(comma_counter == objective)
	    {
		    // Checks to see if the iterator's position is a comma, used to cause a stop in parsing.
		    if(Radio.radio_input[iter] != ',')
		    {
		    	  // Copies the message's character to the temporary array.
		        wanted_data += Radio.radio_input[iter];
		    }
        else
        {
            break;
        }
	    }
  	}
	// Converts the final array to a float and returns.
	return wanted_data.toFloat();
}


/**
 * Responsible for all serial communication between the GUI and mission_control microcontroller.
 */
void DATA::serial_comms()
{
	// Send useful information back to the python GUI.
	update_gui();
}


/**
 * Sends mission_control & craft inforamtion back to the GUI.
 */
void DATA::update_gui()
{
	// Only sends info to update gui every 1/3 second.
	if(!Serial.available() && (millis() - serial_timer >= 1500))
	{
		// Resets / starts timer.
	  serial_timer = millis();
	  // Starts or updates mission control microsecond timer. (Converts to seconds w/ 2 decimal places for easy of use)
	  Radio.mission_control_ts = millis()/1000.0;
    // Holds outgoing message.
		String temp_packet = "";
		temp_packet += "$";
		temp_packet += "/";
		temp_packet += Radio.mission_control_ts;
		temp_packet += "/";
		temp_packet += Radio.radio_input;
		temp_packet += "/";
		temp_packet += Radio.radio_output;
		temp_packet += "/";
		temp_packet += Radio.received_rssi;
		temp_packet += "/";
		temp_packet += "$";

		// Defines a char array with the length needed to hold the received packet.
		char serial_packet[temp_packet.length()+1];
		// Converts from String to char array.
		temp_packet.toCharArray(serial_packet, temp_packet.length()+1);
		// Sends packet via serial to python GUI.
		Serial.write(serial_packet);
	}
}
