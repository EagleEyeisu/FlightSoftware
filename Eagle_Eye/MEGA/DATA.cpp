/**
 * DATA.cpp contains the struct that holds all the Crafts situational information.
 */


#include "DATA.h"
#include "IMU.h"
#include "THERMO.h"
#include "I2C.h"
#include "MOTOR.h"
#include "Globals.h"
#include <Arduino.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_MAX31855.h>


/**
 * Constructor used to reference all other variables & functions.
 */
DATA::DATA()
{
	
}


/**
 *  Retrieves the current Altitude of the craft.
 */
float DATA::get_i2c_current_altitude()
{
 	return Data.Parse(Comm.to_parse,1);
}


/**
 *  Retrieves the current Latitude of the craft.
 */
float DATA::get_i2c_current_latitude()
{
 	return Data.Parse(Comm.to_parse,2);
}


/**
 *  Retrieves the current Longituede of the craft.
 */
float DATA::get_i2c_current_longitude()
{
 	return Data.Parse(Comm.to_parse,3);
}


/**
 *  Retrieves the distance to the target destination.
 */
float DATA::get_i2c_destination_distance()
{
 	return Data.Parse(Comm.to_parse,7);
}


/**
 *  Retrieves the Altitude of the target destination.
 */
float DATA::get_i2c_target_altitude()
{
 	return Data.Parse(Comm.to_parse,4);
}


/**
 *  Retrieves the Latitude of the target destination.
 */
float DATA::get_i2c_target_latitude()
{
 	return Data.Parse(Comm.to_parse,5);
}


/**
 *  Retrieves the Longitude of the target destination.
 */
float DATA::get_i2c_target_longitude()
{
 	return Data.Parse(Comm.to_parse,6);
}


/**
 *  Retrieves the speed (meters per second) of the craft.
 */ 
float DATA::get_i2c_current_speed()
{
 	return Data.Parse(Comm.to_parse,8);
}


/**
 * Retrieves the flight mode of the craft. (Manual or autopilot)
 */ 
float DATA::get_i2c_authority_mode()
{
	return Data.Parse(Comm.to_parse,1); //Actual is 9
}


/**
 * Retrieves the flight mode of the craft. (Manual or autopilot)
 */
float DATA::get_i2c_manual_command()
{
	return Data.Parse(Comm.to_parse,2); //Actual is 9
}


/**
 * Retrieves the flight mode of the craft. (Manual or autopilot)
 */
float DATA::get_i2c_craft_anchor()
{
	return Data.Parse(Comm.to_parse,3); //Actual is 9
}


/**
 * Updates the main struct for the craft. 
 */
void DATA::update_data()
{
	// MEGA DATA
  	Data.set_pressure();
	Local.mega_altitude = Data.calculate_barometer_altitude();
	Local.mega_external_temperature = Thermo.get_external_temperature();
	Local.mega_roll = Imu.get_roll();
	Local.mega_pitch = Imu.get_pitch();
	Local.mega_yaw = Imu.get_yaw();

	Comm.i2c_receive();
	
	// LORA DATA
	if(Comm.complete_packet_flag)
	{
		//Local.lora_current_altitude = Data.get_i2c_current_altitude();
		//Local.lora_current_latitude = Data.get_i2c_current_latitude() / 10000.0;
		//Local.lora_current_longitude = Data.get_i2c_current_longitude() / 10000.0;
		//Local.lora_target_altitude = Data.get_i2c_target_altitude();
		//Local.lora_target_latitude = Data.get_i2c_target_latitude() / 10000.0;
		//Local.lora_target_longitude = Data.get_i2c_target_longitude() / 10000.0;
		//Local.lora_destination_distance = Data.get_i2c_destination_distance();
		//Local.lora_current_speed = Data.get_i2c_current_speed();
		// GENERAL CRAFT / NETWORK DATA
		Local.authority_mode = Data.get_i2c_authority_mode(); 
		Local.craft_manual_direction = Data.get_i2c_manual_command();
		Local.craft_anchor_status = Data.get_i2c_craft_anchor();
	}
}


/**
 * Captures a barometric pressure sample and sets the pressure variable.
 */
void DATA::set_pressure()
{
	// Object used to store the store the data pulled from the BMP085.
	sensors_event_t event;
	// Creates new 'event' with the most current pressure sensor data.
	bmp.getEvent(&event);
	// Updates variable.
	Local.mega_pressure = event.pressure;
}


void DATA::to_screen()
{
	if(new_data == YES)
	{
	    // Prints out data struct to the screen for debugging/following along purposes.
	    Serial.println("-----------------------------------------------------------------------------");
	    Serial.println("-----------------------------------------------------------------------------");
	    Serial.println("-----------------------------------------------------------------------------");
	    Serial.println("|                                MEGA DATA                                  |");
	    Serial.println("|                                                                           |");
	    Serial.print(  "|  Altitude:     "); Serial.print(Local.mega_altitude,2); 			       Serial.print(" m");  Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Temperature:  "); Serial.print(Local.mega_external_temperature);    Serial.print(" C");  Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Pressure:     "); Serial.print(Local.mega_pressure);   			       Serial.print(" hPa");Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Roll:         "); Serial.print(Local.mega_roll);                            				      Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Pitch:        "); Serial.print(Local.mega_pitch);                           				      Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Yaw:          "); Serial.print(Local.mega_yaw);                             				      Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.println("|                                                                           |");
	    Serial.println("-----------------------------------------------------------------------------");
	    Serial.println("|                                LORA DATA                                  |");
	    Serial.println("|                                                                           |");
	    Serial.print(  "|  Flight Mode:   "); Serial.print(Local.authority_mode);                                   Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Cur Altitude:  "); Serial.print(Local.lora_current_altitude,2);    Serial.print(" m");   Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Cur Latitude:  "); Serial.print(Local.lora_current_latitude,5);                          Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Cur Longitude: "); Serial.print(Local.lora_current_longitude,5);                         Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Target Alt:    "); Serial.print(Local.lora_target_altitude,5);                           Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Target Lat:    "); Serial.print(Local.lora_target_latitude,5);                           Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Target Lon:    "); Serial.print(Local.lora_target_longitude,5);                          Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Speed:         "); Serial.print(Local.lora_current_speed);         Serial.print(" mps"); Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Distance:      "); Serial.print(Local.lora_destination_distance);  Serial.print(" m");   Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.println("|                                                                           |");
	    Serial.println("-----------------------------------------------------------------------------");
	    Serial.println("|                                                                           |");
	    Serial.println("|                               MOTOR DATA                                  |");
	    Serial.print(  "|  State:          "); Serial.print(Movement.get_movement_state());		Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Servo Degree:   "); Serial.print(Movement.servo_degree);       		Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Forward:        "); Serial.print(Imu.move_forward);    				    Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Right:          "); Serial.print(Imu.turn_right);      				    Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Left:           "); Serial.print(Imu.turn_left);       				    Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Up:             "); Serial.print(Imu.move_up);         				    Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Down:           "); Serial.print(Imu.move_down);       				    Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Cur Heading:    "); Serial.print(Imu.current_heading);        		  Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Tar Heading:    "); Serial.print(Imu.target_heading);            	Serial.println("\t\t\t\t\t\t\t    |");
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
	// Example GPS Transmission. (GGA)----------------------------------------------------------------------------------
	//
	// $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
	//
	// Example Radio Transmission.--------------------------------------------------------------------------------------
	//
	//                    LORA                                        MISSION CONTROL                       CRAFT ID
	// Time(ms),Altitude,Latitude,Longitude,LE, | Time(ms),craft_anchor,new_throttle,TargetLat,TargetLon, | Signal Origin
	//
	// Example I2C Transmission-----------------------------------------------------------------------------------------
	//
	//                                   CONTROLLER ACCESS NETWORK PROTOCOL PACKET
	// $,GPSAltitude, Latitude, Longitude, TargetAlt, TargetLat, TargetLon, TargetDistance, Speed, Time,$
	//        1           2         3          4          5          6            7           8      9 
	//
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
 * Derives Crafts altitude based on current atmosphereic pressure.
 * DO NOT ALTER THIS METHOD UNLESS EXPLICITLY TOLD TO BY JARED.
 */
float DATA::calculate_barometer_altitude()
{
	// Converts the incoming pressure (hPa) into (mPa).
	float pressure = Local.mega_pressure / 10.0;
	// Altitude value produced from the equations below.
  	// DO NOT ALTER THIS METHOD UNLESS EXPLICITLY TOLD TO BY JARED.
	float result;
	// These variables represent parts of the equation. Think of it like this...
	// (following symbols are not representative of what happens below)   
	//
	//            left_top +- right_top
	//            -------------------     (left_top and right_top combined = top_total)
	//                  bottom
	float left_top;
	float right_top;
	float bottom;
	float top_total;
	// ABOVE 25,000m
  	// DO NOT ALTER THIS METHOD UNLESS EXPLICITLY TOLD BY JARED.
	if (pressure < 2.55)
  	{                         
		left_top = -47454.8;
		right_top = pow(pressure, 0.087812) - 1.65374;
		bottom = pow(pressure, 0.087812);
		top_total = left_top * right_top;
		result = (top_total / bottom);
	}
	// Between 25,000m and 11,000m
  	// DO NOT ALTER THIS METHOD UNLESS EXPLICITLY TOLD BY JARED.
	else if(67.05 > pressure && pressure > 2.55)
  	{
		right_top = -6369.43;
		left_top = log(pressure) - 4.85016;
		result =  left_top * right_top;
	}
	// BELOW 11,000m (Pressure > 67.05)
  	// DO NOT ALTER THIS METHOD UNLESS EXPLICITLY TOLD BY JARED.
	else
  	{
		left_top = 44397.5;
		right_top = 18437 * pow(pressure, 0.190259);
		result = left_top - right_top;
	}
	return result;
}


/**
 * Tests connection to Barometer.
 */
void DATA::initialize()
{
	if(!bmp.begin())
 	{
		Serial.println("PROBLEM WITH PRESSURE SENSOR.");

    while(1);
    
	}
	//Valid connection, program proceeds as planned.
	else
  	{
		Serial.println("Pressure Sensor Online.");
	}
}
