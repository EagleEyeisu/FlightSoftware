/**
 * Save.cpp fills out the 'time capsule' with all the current data and saves it to the SD cards.
 *    This happens every time the program cycles. This replaces the old way of saving every individual
 *    event/altitude/etc.. Now is it saved in one decently long string that is seperated by commas -> ','
 */


#include <Arduino.h>
#include "Data.h"
#include "Save.h"
#include "SD.h"


/**
 * Constructor used to reference all other variables & functions.
 */
Save::Save()
{
	
}


/**
 * Manages the saving process. Constructs the below array and saves to the SD Card. 
 *
 *    Capsule -> "Altitude, Latitude, Longitude, GPS_Fix, Satellite_Count, LoRa Event, Mega Event"
 *                    1        2         3          4              5            6           7
 */
void Save::Save_Data(class Data_in, class GPS_in, class I2C_in, class Para_in, class Radio_in, class Save_in)
{
  Data = Data_in;
  GPS = GPS_in;
  I2C = I2C_in;
  Para = Para_in;
  Radio = Radio_in;
  //Save = Save_in;
	
	//All flight data is consolidated into one char array. All values are casted (turned into) to chars
  //   to be saved.
	char temp[] = {char(Data.Local.Altitude),
                 ',',
      					 char(Data.Local.Latitude),
                 ',',
      					 char(Data.Local.Longitude),
                 ',',
      					 char(Data.Local.GPS_Fix),
                 ',',
      					 char(Data.Local.Satellite_Count),
                 ',',
      					 char(Data.Local.LE),
                 ',',
      					 char(Data.Local.ME)
      					};

  //File object used to access the SD card.
  File Storage;
  
	//Opens up SD card and saves the data capsule.
	Storage = SD.open("LORA_FltData", FILE_WRITE);
	Storage.println(temp);
	Storage.close();
	
	//Resets Local.LE & Local.ME to prevent repeated recording of the same event over multiple cycles.
	Data.Local.LE = 0;
	Data.Local.ME = 0;
}


/**
 * Assigns correct pin to connect to the SD card. Tests connection.
 */
void Save::Initialize()
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


