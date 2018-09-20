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
 	return Data.Parse(Comm.NDP,1);
}


/**
 *  Retrieves the current Latitude of the craft.
 */
float DATA::get_i2c_current_latitude()
{
 	return Data.Parse(Comm.NDP,2);
}


/**
 *  Retrieves the current Longituede of the craft.
 */
float DATA::get_i2c_current_longitude()
{
 	return Data.Parse(Comm.NDP,3);
}


/**
 *  Retrieves the distance to the target destination.
 */
float DATA::get_i2c_current_distance()
{
 	return Data.Parse(Comm.NDP,7);
}


/**
 *  Retrieves the Altitude of the target destination.
 */
float DATA::get_i2c_destination_altitude()
{
 	return Data.Parse(Comm.NDP,4);
}


/**
 *  Retrieves the Latitude of the target destination.
 */
float DATA::get_i2c_destination_latitude()
{
 	return Data.Parse(Comm.NDP,5);
}


/**
 *  Retrieves the Longitude of the target destination.
 */
float DATA::get_i2c_destination_longitude()
{
 	return Data.Parse(Comm.NDP,6);
}


/**
 *  Retrieves the speed (meters per second) of the craft.
 */ 
float DATA::get_i2c_current_speed()
{
 	return Data.Parse(Comm.NDP,8);
}


/**
 * Updates the MEGA's copy of the time stamp onbaord the LoRa. (From GPS device)
 */ 
void DATA::get_i2c_current_timestamp()
{
	// This is a junk variable. This method returns nothing, but must be called to update
	// the time variable. Refer to the parse method in Data.cpp for more details.
	float junk = Data.Parse(Comm.NDP,9);
}


/**
 * Updates the main struct for the craft. 
 */
void DATA::update_data()
{
	// Object used to store the store the data pulled from the BMP085.
	sensors_event_t event;
	// Creates new 'event' with the most current pressure sensor data.
	bmp.getEvent(&event);
	// MEGA DATA
	Local.current_altitude = Data.getAltitude(event.pressure);
	Local.Latitude = Data.getGPSLatitude() / 10000.0;
	Local.Longitude = Data.getGPSLongitude() / 10000.0;
	Local.TempExt = Thermo.getTempExt();
	Local.Pressure = event.pressure;
	Local.Roll = Imu.getRoll();
	Local.Pitch = Imu.getPitch();
	Local.Yaw = Imu.getYaw();
	// LORA DATA
	Local.GPSAltitude = Data.getGPSAltitude();
	Local.GPSTargetAlt = Data.getGPSTargetAlt();
	Local.GPSTargetLat = Data.getGPSTargetLat() / 10000.0;
	Local.GPSTargetLon = Data.getGPSTargetLon() / 10000.0;
	Local.GPSTargetDistance = Data.getGPSTargetDistance();
	Local.GPSSpeed = Data.getGPSSpeed();
	Data.getGPSTime();
}


void DATA::to_screen()
{
	if(newData == YES)
	{
	    // Prints out data struct to the screen for debugging/following along purposes.
	    Serial.println("-----------------------------------------------------------------------------");
	    Serial.println("-----------------------------------------------------------------------------");
	    Serial.println("-----------------------------------------------------------------------------");
	    Serial.println("|                                MEGA DATA                                  |");
	    Serial.println("|                                                                           |");
	    Serial.print(  "|  Altitude:     "); Serial.print(Local.Altitude,2); Serial.print(" m");  Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Temperature:  "); Serial.print(Local.TempExt);    Serial.print(" C");  Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Pressure:     "); Serial.print(Local.Pressure);   Serial.print(" hPa");Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Roll:         "); Serial.print(Local.Roll);                            Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Pitch:        "); Serial.print(Local.Pitch);                           Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Yaw:          "); Serial.print(Local.Yaw);                             Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  LoRa Event:   "); Serial.print(Local.LE);                              Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Mega Event:   "); Serial.print(Local.ME);                              Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.println("|                                                                           |");
	    Serial.println("-----------------------------------------------------------------------------");
	    Serial.println("|                                LORA DATA                                  |");
	    Serial.println("|                                                                           |");
	    Serial.print(  "|  Time:         "); Serial.print(Local.GPSTime);                                Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  GPS Altitude: "); Serial.print(Local.GPSAltitude,2);     Serial.print(" m");  Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Latitude:     "); Serial.print(Local.Latitude,5);                             Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Longitude:    "); Serial.print(Local.Longitude,5);                            Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  TargetLat:    "); Serial.print(Local.GPSTargetLat,5);                         Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  TargetLon:    "); Serial.print(Local.GPSTargetLon,5);                         Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Speed:        "); Serial.print(Local.GPSSpeed);          Serial.print(" mps");Serial.println("\t\t\t\t\t\t    |");
	    Serial.print(  "|  Distance:     "); Serial.print(Local.GPSTargetDistance); Serial.print(" m");  Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.println("|                                                                           |");
	    Serial.println("-----------------------------------------------------------------------------");
	    Serial.println("|                                                                           |");
	    Serial.println("|                               MOTOR DATA                                  |");
	    Serial.print(  "|  State:        "); Serial.print(Movement.getSTATE());Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Servo Pos   : "); Serial.print(Movement.pos);       Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Move Forward: "); Serial.print(Imu.moveForward);    Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Turn Right:   "); Serial.print(Imu.turnRight);      Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Turn Left:    "); Serial.print(Imu.turnLeft);       Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Move Up:      "); Serial.print(Imu.moveUp);         Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Move Down:    "); Serial.print(Imu.moveDown);       Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Target Angle: "); Serial.print(Imu.ATT);            Serial.println("\t\t\t\t\t\t\t    |");
	    Serial.print(  "|  Bearing:      "); Serial.print(Imu.bearing);        Serial.println("\t\t\t\t\t\t\t    |");
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
  	for(i=0; i<120; i++)
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
	char time_filer[temp_iter];
	// Iterates through the temporary array copying over the info to the variable which will be returned.
	for(i=0; i<temp_iter; i++)
	{
		// Copying of the information between arrays.
		time_filer[i] = temp_array[i];
	}
	// Used below to detect the parsing of the time value.
	bool time_detected = false;
	// Cycles through to check for if the time value is being parsed.
	// The time value is saved as a short char array, where as all
	// other parsed values are converted to floats before being 
	// returned.
	for(i=0; i<temp_iter; i++)
	{
		// The time character array will have colons located in it.
		// No other value being parsed should contain this character.
		// Therefore, if colons are detected, time is present.
		if(time_filer[i]==':')
		{
			time_detected = true;
		}
	}
	// Time value has been detected.
	if(time_detected)
	{
		// Resets the variable for redundancy.
		time_detected = false;
		// Copies over time_filer to time.
		for(i=0; i<temp_iter; i++)
		{
			Data.Local.GPSTime[i] = time_filer[i];
		}
	}
	// Time value is not detected. Converts to float and returns.
	else
	{	
		// Converts the final array to a float and returns.
		return atof(time_filer);
	}
}


/**
 * Derives Crafts altitude based on current atmosphereic pressure.
 * DO NOT ALTER THIS METHOD UNLESS EXPLICITLY TOLD TO BY JARED.
 */
float DATA::getAltitude(float _Pressure)
{
	// Converts the incoming pressure (hPa) into (mPa).
	float pressure = _Pressure / 10.0;
	// Altitude value produced from the equations below.
  	// DO NOT ALTER THIS METHOD UNLESS EXPLICITLY TOLD TO BY JARED.
	float alt;
	// These variables represent parts of the equation. Think of it like this...
	// (following symbols are not representative of what happens below)   
	//
	//            leftTop +- rightTop
	//            -------------------     (leftTop and rightTop combined = topTotal)
	//                  bottom
	float leftTop;
	float rightTop;
	float bottom;
	float topTotal;
	// ABOVE 25,000m
  	// DO NOT ALTER THIS METHOD UNLESS EXPLICITLY TOLD BY JARED.
	if (pressure < 2.55)
  	{                         
		leftTop = -47454.8;
		rightTop = pow(pressure, 0.087812) - 1.65374;
		bottom = pow(pressure, 0.087812);
		topTotal = leftTop * rightTop;
		alt = (topTotal / bottom);
	}
	// Between 25,000m and 11,000m
  	// DO NOT ALTER THIS METHOD UNLESS EXPLICITLY TOLD BY JARED.
	else if(67.05 > pressure && pressure > 2.55)
  	{
		rightTop = -6369.43;
		leftTop = log(pressure) - 4.85016;
		alt =  leftTop * rightTop;
	}
	// BELOW 11,000m (Pressure > 67.05)
  	// DO NOT ALTER THIS METHOD UNLESS EXPLICITLY TOLD BY JARED.
	else
  	{
		leftTop = 44397.5;
		rightTop = 18437 * pow(pressure, 0.190259);
		alt = leftTop - rightTop;
	}
	return alt;
}


/**
 * Tests connection to Barometer.
 */
void DATA::initialize()
{
	// If invalid connection, the program will stall and print an error message.
  	Serial.println("In Data");
	if(!bmp.begin())
 	{
		Serial.println("PROBLEM WITH PRESSURE SENSOR.");
		
	}
	//Valid connection, program proceeds as planned.
	else
  	{
		Serial.println("Pressure Sensor Online.");
	}
}
