/**
 * Save.cpp fills out the 'time capsule' with all the current data and saves it to the SD cards.
 *    This happens every time the program cycles. This replaces the old way of saving every individual
 *    event/altitude/etc.. Now is it saved in one decently long string that is seperated by commas -> ','
 */


#include "Save.h"


/**
 * Manages the saving process. Constructs the below array and saves to the SD Card. 
 */
void Save::Save_Data()
{
	
	//Creates temporary array of floats and fills it with appropriate info.
	float[] temp = {
					Network.L_TS,
				    Network.Altitude,
				    Network.Latitude,
				    Network.Longitude,
				    Network.LE,
				    Network.H_TS,
				    Network.Release_Status,
				    Network.MS_TS,
				    Network.Command_Sent,
					Network.Command_Received,
					Network.Craft_ID
					};
		
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


