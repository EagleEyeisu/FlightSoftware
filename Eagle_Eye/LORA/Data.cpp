/**
 * DATA.cpp contains the struct that holds all the Crafts situational information.
 */


#include <Arduino.h>
#include "Data.h"
#include "Radio.h"
#include "GPS.h"
#include <stdlib.h>
#include "Globals.h"


/**
 * Constructor used to reference all other variables & functions.
 */
DATA::DATA()
{
	
}


/**
 * Updates the main struct for the craft. 
 */
void DATA::update_data()
{
	// Data that is native to the MEGA microcontroller.
  	Data.set_pressure();
	Local.mega_altitude = Data.calculate_barometer_altitude();
	Local.mega_external_temperature = Thermo.get_external_temperature();
	Local.mega_roll = Imu.get_roll();
	Local.mega_pitch = Imu.get_pitch();
	Local.mega_yaw = Imu.get_yaw();
  	
	// Data that is coming into the MEGA via i2c. Checks for a complete
	// packet flag. If true, the packet is valid and ready to be parsed.
	if(Comm.flag_complete_packet)
	{
		// Converts the string packet into a character array.
		// (Makes it easier to work with).
		char to_parse[Comm.i2c_input_buffer.length()];
		// Indexed at 0 so we need to add 1 at the end of the length.
    	Comm.i2c_input_buffer.toCharArray(to_parse,Comm.i2c_input_buffer.length()+1);
    	// Checks for i2c packet type of Current data.
		if(to_parse[2] == 'C')
		{
			// Methods located in Data.cpp. Parses appropriate values from packet.
			Local.lora_current_altitude = Data.get_i2c_current_altitude();
			Local.lora_current_latitude = Data.get_i2c_current_latitude() / 10000.0;
			Local.lora_current_longitude = Data.get_i2c_current_longitude() / 10000.0;
			Local.lora_current_speed = Data.get_i2c_current_speed();
		}
		// Checks for i2c packet type of Target data.
		else if(to_parse[2] == 'T')
		{
			// Methods located in Data.cpp. Parses appropriate values from packet.
			Local.lora_target_altitude = Data.get_i2c_target_altitude();
			Local.lora_target_latitude = Data.get_i2c_target_latitude() / 10000.0;
			Local.lora_target_longitude = Data.get_i2c_target_longitude() / 10000.0;
			Local.lora_target_distance = Data.get_i2c_target_distance();
		}
		// Checks for i2c packet type of Network data.
		else if(to_parse[2] == 'N')
		{
			// Methods located in Data.cpp. Parses appropriate values from packet.
			Local.authority_mode = Data.get_i2c_authority_mode(); 
			Data.Local.lora_target_throttle = Data.get_i2c_target_throttle(to_parse);
			Data.Local.craft_manual_direction = Data.get_i2c_manual_command(to_parse);
			Local.craft_anchor_status = Data.get_i2c_craft_anchor();
		}
	}
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
	// Time(ms),Altitude,Latitude,Longitude,LE, | Time(ms),Start_Stop,new_throttle,TargetLat,TargetLon, | Signal Origin
	//
	// Example I2C Transmission
	//
	//                                  CONTROLLER ACCESS NETWORK PROTOCOL PACKET
	// $,GPSAltitude, Latitude, Longitude, TargetLat, TargetLon, Roll, Pitch, Yaw, Speed, TargetDistance, Time,$
	//        1           2         3          4           5       6     7     8     9         10          11
	//
	// The number of commas that the program needs to pass before it started parsing the data.
  	
	// Used to iterate through the passed in character array.
	int i = 0;
	// This iterator is used to pull the wanted part of the 'message' from the entire array.
	// Used to gather information such as how long the new parsed section is.
	int temp_iter = 0;
	// Counts the commas as the character array is iterated over.
	int comma_counter = 0;
	// Temporary string used to hold the newly parsed array.
	char temp_array[20];
	// Iterators over the entire array.
	for(i=0; i<150; i++)
	{
		// Checks to see if the current iterator's position is a comma. 
		if(message[i] == ',')
		{
			// If so, it iterators the comma counter by 1.
			comma_counter++;
		}
		// Checks to see if the desired amount of commas has been passed. 
		else if(comma_counter == objective)
		{
			// Checks to see if the iterator's position is a comma, used to cause a stop in parsing.
			if(message[i] != ',')
			{
				// Copies the message's character to the temporary array.
				temp_array[temp_iter] = message[i];
				// Iterator used to tell how long the temporary array is.
				temp_iter++;
			}
		}
	}
	// Charater array used with a fitted length of the parsed section.
	char parsed_section[temp_iter];
	// Iterates through the temporary array copying over the info to the variable which will be returned.
	for(i=0; i<temp_iter; i++)
	{
		// Copying of the information between arrays.
		parsed_section[i] = temp_array[i];
	}
	// Converts the final array to a float.
	float temp = atof(parsed_section);
	// Returns the desired parsed section in number (float) form.
	return temp;
}
