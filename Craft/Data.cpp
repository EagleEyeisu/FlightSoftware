/**
 * DATA.cpp contains the struct that holds all the Crafts situational information.
 */


#include <Arduino.h>
#include <stdlib.h>
#include <Adafruit_MAX31855.h>
#include <SD.h>
#include <SPI.h>

#include "Data.h"
#include "Radio.h"
#include "GPS.h"
#include "IMU.h"
#include "Globals.h"
#include "Thermo.h"
#include "MOTOR.h"


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
            //blink_error_led(); 
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
	//log_data();
	// Read in temperature data.
	external_temperature = Thermo.get_external_temperature();
	// Read in IMU data.
	Imu.roll = Imu.get_roll();
	Imu.pitch = Imu.get_pitch();
	Imu.yaw = Imu.get_yaw();
	// Display new data to screen. 
	to_screen();
}


void DATA::to_screen()
{
    if(millis() - display_timer > 2000)
    {
        // Reset timer.
        display_timer = millis();
        // Prints out data struct to the screen for debugging/following along purposes.
        Serial.println("-----------------------------------------------------------------------------");
        Serial.println("|                               Radio Data                                  |");
        Serial.print(  "|  Radio I:      "); Serial.print(Radio.radio_input);           Serial.println("\t\t\t\t\t\t\t    |");
        Serial.print(  "|  Radio O:      "); Serial.print(Radio.radio_output);          Serial.println("\t\t\t\t\t\t\t    |");
        Serial.println("|                                                                           |");
        Serial.println("-----------------------------------------------------------------------------");
        Serial.println("|                                Positional                                 |");
        Serial.println("|                                                                           |");
        Serial.print(  "|  Temperature:  "); Serial.print(external_temperature);        Serial.print(" C");  Serial.println("\t\t\t\t\t\t    |");
        Serial.print(  "|  Roll:         "); Serial.print(Imu.roll);                    Serial.println("\t\t\t\t\t\t\t    |");
        Serial.print(  "|  Pitch:        "); Serial.print(Imu.pitch);                   Serial.println("\t\t\t\t\t\t\t    |");
        Serial.print(  "|  Yaw:          "); Serial.print(Imu.yaw);                     Serial.println("\t\t\t\t\t\t\t    |");
        Serial.println("|                                                                           |");
        Serial.println("-----------------------------------------------------------------------------");
        Serial.println("|                                GPS Data                                   |");
        Serial.println("|                                                                           |");
        Serial.print(  "|  Cur Altitude:  "); Serial.print(Gps.gps_altitude,2);         Serial.print(" m");   Serial.println("\t\t\t\t\t\t\t    |");
        Serial.print(  "|  Cur Latitude:  "); Serial.print(Gps.gps_latitude,5);         Serial.println("\t\t\t\t\t\t    |");
        Serial.print(  "|  Cur Longitude: "); Serial.print(Gps.gps_longitude,5);        Serial.println("\t\t\t\t\t\t    |");
        Serial.print(  "|  Target Alt:    "); Serial.print(Gps.target_altitude,5);      Serial.println("\t\t\t\t\t\t    |");
        Serial.print(  "|  Target Lat:    "); Serial.print(Gps.target_latitude,5);      Serial.println("\t\t\t\t\t\t    |");
        Serial.print(  "|  Target Lon:    "); Serial.print(Gps.target_longitude,5);     Serial.println("\t\t\t\t\t\t    |");
        Serial.print(  "|  Speed:         "); Serial.print(Gps.gps_speed);              Serial.print(" mps"); Serial.println("\t\t\t\t\t\t    |");
        Serial.print(  "|  Distance:      "); Serial.print(Gps.gps_distance);           Serial.print(" m");   Serial.println("\t\t\t\t\t\t\t    |");
        Serial.println("|                                                                           |");
        Serial.println("-----------------------------------------------------------------------------");
        Serial.println("|                                                                           |");
        Serial.println("|                               MOTOR DATA                                  |");
        Serial.print(  "|  State:          "); Serial.print(Movement.get_movement_state());   Serial.println("\t\t\t\t\t\t\t    |");
        Serial.print(  "|  Tar Throttle:   "); Serial.print(Radio.target_throttle);           Serial.println("\t\t\t\t\t\t\t    |");
        Serial.print(  "|  Forward:        "); Serial.print(Imu.move_forward);                Serial.println("\t\t\t\t\t\t\t    |");
        Serial.print(  "|  backward:       "); Serial.print(Imu.move_backward);               Serial.println("\t\t\t\t\t\t\t    |");
        Serial.print(  "|  Right:          "); Serial.print(Imu.turn_right);                  Serial.println("\t\t\t\t\t\t\t    |");
        Serial.print(  "|  Left:           "); Serial.print(Imu.turn_left);                   Serial.println("\t\t\t\t\t\t\t    |");
        Serial.print(  "|  Cur Heading:    "); Serial.print(Imu.craft_heading);               Serial.println("\t\t\t\t\t\t\t    |");
        Serial.print(  "|  Tar Heading:    "); Serial.print(Imu.target_heading);              Serial.println("\t\t\t\t\t\t\t    |");
        Serial.println("|                                                                           |");
        Serial.println("-----------------------------------------------------------------------------");
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
        // Checks for valid file.
        if(sd_card)
        {
            sd_card.print("Radio In: ");
            sd_card.println(Radio.radio_input);
            sd_card.print("Radio Out: ");
            sd_card.println(Radio.radio_output);
            
            // Pulse onboard Green LED. 
            digitalWrite(8, HIGH);
            delay(100);
            // Turn off LED.
            digitalWrite(8, LOW);
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
