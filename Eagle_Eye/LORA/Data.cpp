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
	// parameter objective ^^^
  
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
	for(i=0;i<150;i++)
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
 * Prints information to screen. 
 */
void DATA::displayInfo()
{
  if(newData == YES)
  {
    // Prints out data struct to the screen for debugging/following along purposes.
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println("|                                Craft Data                                 |");
    Serial.println("|                                                                           |");
    Serial.print(  "|  Received: ");  Serial.print(Radio.radioInput);  Serial.println("");
    Serial.print(  "|  Sent:     ");  Serial.print(Radio.radioOutput); Serial.println("");
    Serial.println("|                                                                           |");
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println("|                                                                           |");
    Serial.print(  "|  Operation Mode:   "); Serial.print(Radio.getOpSTATE()); Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  Roll Call Status: "); Serial.print(Radio.getRCSTATE()); Serial.println("\t\t\t\t\t\t    |");
    Serial.println("|                                                                           |");
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println("|                                                                           |");
    Serial.print(  "|  Lora Time Stamp:  "); Serial.print(Radio.Network.L_TS);  Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  Altitude:   "); Serial.print(Radio.Network.Altitude);    Serial.print(" m"); Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  Latitude:   "); Serial.print(Radio.Network.Latitude,5);  Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  Longitude:  "); Serial.print(Radio.Network.Longitude,5); Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  LoRa Event: "); Serial.print(Radio.Network.LE);          Serial.println("\t\t\t\t\t\t\t    |");
    Serial.println("|                                                                           |");
    Serial.println("-----------------------------------------------------------------------------");
    Serial.println("|                                                                           |");
    Serial.print(  "|  Mission Control Time Stamp: "); Serial.print(Radio.Network.MC_TS);          Serial.println("\t\t\t\t\t    |");
    Serial.print(  "|  Operational Status: ");         Serial.print(Radio.getFunctionalSTATE());   Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  Target Throttle: ");            Serial.print(Radio.Network.TargetThrottle); Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  Target Lat: ");                 Serial.print(Radio.Network.TargetLon,5);    Serial.println("\t\t\t\t\t\t    |");
    Serial.print(  "|  Target Lon: ");                 Serial.print(Radio.Network.TargetLat,5);    Serial.println("\t\t\t\t\t\t    |");
    Serial.println("|                                                                           |");
    Serial.println("-----------------------------------------------------------------------------");
    Serial.print(  "|  Received ID:  ");  Serial.print(Radio.receivedID); Serial.println("\t\t\t\t\t\t\t    |");
    Serial.println("-----------------------------------------------------------------------------");
  
    // Resets the newData state to no new data.
    Data.newData = Data.NO;
  }
}
