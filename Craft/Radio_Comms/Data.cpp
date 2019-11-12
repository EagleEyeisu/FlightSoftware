/**
 * DATA.cpp contains the struct that holds all the Crafts situational information.
 */


#include <Arduino.h>
#include "Data.h"
#include "Radio.h"
#include "GPS.h"
#include "UART.h"
#include <stdlib.h>
#include "Globals.h"


/**
 * Constructor used to reference all other variables & functions.
 */
DATA::DATA()
{
	
}


/**
 * Initialize the SD card and status LEDs.
 */
void DATA::initialize()
{
    // Check for valid connection to SD card.
    if(!SD.begin(SD_CS))
    {
        // Invalid connection.
        while(1)
        {
            blink_error_led(); 
            Serial.println("SD Failed");
        }
    }
}


/**
 * Manages the bootup process and status leds.
 */
void DATA::manager()
{
	// Stores data to sd card.
	log_data();
}


/*--------------------UART NETWORK PACKET (W)--------------------*/

/**
 * Retrieves the flight controller's pressure value.
 */
float DATA::get_uart_fltctrl_pressure(char buf[])
{
	return Data.Parse(buf,2);
}


/**
 * Retrieves the flight controller's altitude value (calculated by pressure).
 */
float DATA::get_uart_fltctrl_altitude(char buf[])
{
 	return Data.Parse(buf,3);
}


/**
 * Retrieves the flight controller's temperature value (external temp).
 */
float DATA::get_uart_fltctrl_temp(char buf[])
{
 	return Data.Parse(buf,4);
}


/*--------------------UART NETWORK PACKET (G)--------------------*/

/**
 * Retrieves the flight controller's roll value.
 */
float DATA::get_uart_fltctrl_roll(char buf[])
{
 	return Data.Parse(buf,2);
}


/**
 * Retrieves the flight controller's pitch value.
 */
float DATA::get_uart_fltctrl_pitch(char buf[])
{
 	return Data.Parse(buf,3);
}


/**
 * Retrieves the flight controller's yaw value.
 */
float DATA::get_uart_fltctrl_yaw(char buf[])
{
 	return Data.Parse(buf,4);
}


/*--------------------UART NETWORK PACKET (P)--------------------*/

/**
 * Retrieves the flight controller's target heading angle.
 */
float DATA::get_uart_target_heading(char buf[])
{
 	return Data.Parse(buf,2);
}


/**
 * Retrieves the flight controller's current heading angle.
 */
float DATA::get_uart_current_heading(char buf[])
{
 	return Data.Parse(buf,3);
}


/**
 * Retrieves the flight controller's craft state.
 * Possible values:
 * 0 : NONE
 * 1 : RIGHT
 * 2 : LEFT
 * 3 : FORWARD
 * 4 : UP
 * 5 : BREAK
 */
float DATA::get_uart_craft_state(char buf[])
{
 	return Data.Parse(buf,4);
}


/**
 * Updates the main struct for the craft.
 */
void DATA::update_data()
{
	// Data that is coming into the MEGA via UART. Checks for a complete
	// packet flag. If true, the packet is valid and ready to be parsed.
	if(Comm.flag_complete_packet)
	{
		// Converts the string packet into a character array.
		// (Makes it easier to work with).
		char to_parse[Comm.uart_input_buffer.length()];
		// Indexed at 0 so we need to add 1 at the end of the length.
    	Comm.uart_input_buffer.toCharArray(to_parse,Comm.uart_input_buffer.length()+1);
    	// Checks for UART packet type of Weather data.
		if(to_parse[2] == 'W')
		{
			// Methods located in Data.cpp. Parses appropriate values from packet.
			fltctrl_pressure = Data.get_uart_fltctrl_pressure(to_parse);
			fltctrl_altitude = Data.get_uart_fltctrl_altitude(to_parse);
			fltctrl_external_temperature = Data.get_uart_fltctrl_temp(to_parse);
		}
		// Checks for UART packet type of Gryo data.
		else if(to_parse[2] == 'G')
		{
			// Methods located in Data.cpp. Parses appropriate values from packet.
			fltctrl_roll = Data.get_uart_fltctrl_roll(to_parse);
			fltctrl_pitch = Data.get_uart_fltctrl_pitch(to_parse);
			fltctrl_yaw = Data.get_uart_fltctrl_yaw(to_parse);
		}
		// Checks for UART packet type of Positioning data.
		else if(to_parse[2] == 'P')
		{
			// Methods located in Data.cpp. Parses appropriate values from packet.
			target_heading = Data.get_uart_target_heading(to_parse);
			craft_heading = Data.get_uart_current_heading(to_parse);
			craft_state = Data.get_uart_craft_state(to_parse);
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
	// Example UART Transmission
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


/**
 * Logs data to the SD card.
 */
void DATA::log_data()
{
  if(millis() - sd_timer > 1000)
  {
    sd_timer = millis();
    // Open & Save.
    File sd_card;
    sd_card = SD.open("Radio_Traffic", FILE_WRITE);
    if(sd_card)
    {
      sd_card.print("Radio In: ");
      sd_card.println(Radio.radio_input);
      sd_card.print("Radio Out: ");
      sd_card.println(Radio.radio_output);
    }
    sd_card.close();
  }
}


/**
 * Blinks the external led referring to the receive led.
 */
void DATA::blink_receive_led()
{
    // ON
    //digitalWrite(RECEIVE_LED, HIGH);
    delay(100);
    // OFF
    //digitalWrite(RECEIVE_LED, LOW);
}


/**
 * Blinks the external led referring to the sending led.
 */
void DATA::blink_send_led()
{
    // ON
    //analogWrite(SEND_LED, HIGH);
    delay(100);
    // OFF
    //analogWrite(SEND_LED, LOW);
}


/*
 * Blinks LED on error.
 */
void DATA::blink_error_led()
{
    // ON
    //digitalWrite(ERROR_LED, HIGH);
    delay(100);
    // OFF
    //digitalWrite(ERROR_LED, LOW);
}
