/**
 * Save.h declares functions and varaibles used in Storage.cpp.
 */

#ifndef Save_h
#define Save_h

#include <Arduino.h>

class Save
{
  public:
	
	//Constructor
	Save();
	
	//Saves struct (current flight data), to SD card in correct format.
	void Save_Data(class Data_in, class GPS_in, class I2C_in, class Para_in, class Radio_in, class Save_in);
		
	  //Runs initialzation script for the SD card.
	void Initialize();
		


  //Holds the passed in class objects to gain access to their references.
  class Data;
  class GPS;
  class I2C;
  class Para;
  class Radio;
  //class Save;
  
	//Digital connection pin for SD card. 
	#define SD_PIN 10

};
#endif
