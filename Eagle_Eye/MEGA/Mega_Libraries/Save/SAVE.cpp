/**
 * Save.cpp fills out the 'time capsule' with all the current data and saves it to the SD cards.
 *    This happens every time the program cycles. This replaces the old way of saving every individual
 *    event/altitude/etc.. Now is it saved in one decently long string that is seperated by commas -> ','
 */


#include "Save.h"


/**
 * Manages the saving process. For a more detailed explanation, bug James. 
 *
 *    Capsule -> "Altitude, TempExt, Pressure, Roll, Pitch, Yaw, LoRa Event, Mega Event"
 *                    1        2         3       4     5     6       7           8
 */
void Save::Save_Data()
{
	
	//New string defined as NULL.
	String Capsule = "";
	
	//All flight data information is appended.
	Capsule += data.Altitude;
	Capsule += ",";
	Capsule += data.TempExt;
	Capsule += ",";
	Capsule += data.Pressure;
	Capsule += ",";
	Capsule += data.Roll;
	Capsule += ",";
	Capsule += data.Pitch;
	Capsule += ",";
	Capsule += data.Yaw;
	Capsule += ",";
	Capsule += data.LE;
	Capsule += ",";
	Capsule += data.ME;
	
	//Opens up SD card and saves the data capsule.
	SDCard = SD.open("MEGA_FltData", FILE_WRITE);
	SDCard.println(Capsule);
	SDCard.close();
	
	//Resets data.LE & data.ME to prevent repeated recording of the same event over multiple cycles.
	data.LE = 0;
	data.ME = 0;
}


/**
 * Assigns correct pin to connect to the SD card. Tests connection.
 */
void Save::SD_Initialize()
{
	//Sets GPIO pin 53 to communicate to the SD card.
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