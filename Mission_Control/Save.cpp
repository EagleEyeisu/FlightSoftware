/**
 * Save.cpp fills out the 'time capsule' with all the current data and saves it to the SD cards.
 *    This happens every time the program cycles. This replaces the old way of saving every individual
 *    event/altitude/etc.. Now is it saved in one decently long string that is seperated by commas -> ','
 */

#include <Arduino.h>
#include "Save.h"
#include "Data.h"
#include <SD.h>
#include "Globals.h"


/**
 * Constructor used to reference all other variables & functions.
 */
SAVE::SAVE()
{
  
}


/**
 * Manages the saving process. Constructs the below array and saves to the SD Card. 
 */
void Save::Save_Data()
{
	
	//Creates temporary array of floats and fills it with appropriate info.
	float[] temp = {char(Network.L_TS),
                  ',',
      				    char(Network.Altitude),
                  ',',
      				    char(Network.Latitude),
                  ',',
      				    char(Network.Longitude),
                  ',',
      				    char(Network.LE),
                  ',',
      				    char(Network.H_TS),
                  ',',
      				    char(Network.Release_Status),
                  ',',
      				    char(Network.MS_TS),
                  ',',
      				    char(Network.Command_Sent),
                  ',',
      					  char(Network.Command_Received),
                  ',',
      					  char(Network.Craft_ID)
      					};

  //File object used to access the SD card.
  File Storage;
  
	//Opens up SD card and saves the data capsule.
	SDCard = SD.open("MS_FltData", FILE_WRITE);
	SDCard.println(temp);
	SDCard.close();
}


/**
 * Assigns correct pin to connect to the SD card. Tests connection.
 */
void Save::SD_Initialize()
{
	//Sets GPIO pin 10 to communicate to the SD card.
	pinMode(SD_PIN, OUTPUT);
	
	//Checks for physical and data connection through the pin assigned above.
	//   If invalid connection, the program will stall and print an error message.
	if(!SD.begin(SD_PIN)){
		Serial.println("PROBLEM WITH SD CARD.");
		while(1);
	}
	//Valid connection, program proceeds as planned.
	else{
		Serial.println("SD Card Online.");
	}
}


