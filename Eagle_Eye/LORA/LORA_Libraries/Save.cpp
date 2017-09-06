/**
 * Save.cpp fills out the 'time capsule' with all the current data and saves it to the SD cards.
 *    This happens every time the program cycles. This replaces the old way of saving every individual
 *    event/altitude/etc.. Now is it saved in one decently long string that is seperated by commas -> ','
 */


#include "Save.h"


/**
 * Manages the saving process. Constructs the below array and saves to the SD Card. 
 *
 *    Capsule -> "Altitude, Latitude, Longitude, GPS_Fix, Satellite_Count, LoRa Event, Mega Event"
 *                    1        2         3          4              5            6           7
 */
void Save::Save_Data()
{
	
	//All flight data information is appended.
	float[] temp = {Local.Altitude,
					Local.Latitude,
					Local.Longitude,
					Local.GPS_Fix,
					Local.Satellite_Count,
					Local.LE,
					Local.ME
					};
	
	//Opens up SD card and saves the data capsule.
	SDCard = SD.open("LORA_FltData", FILE_WRITE);
	SDCard.println(temp);
	SDCard.close();
	
	//Resets Local.LE & Local.ME to prevent repeated recording of the same event over multiple cycles.
	Local.LE = 0;
	Local.ME = 0;
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


