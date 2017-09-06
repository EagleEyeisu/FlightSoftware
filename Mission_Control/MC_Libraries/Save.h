/**
 * Save.h declares functions and varaibles used in Storage.cpp.
 */

#ifndef Save_h
#define Save_h

class Save
{
  public:
  
	//Saves struct (current flight data), to SD card in correct format.
	void Save_Data();
	
  	//Runs initialzation script for the SD card.
	void SD_Initialize();
	

	
	//File object used to access the SD card.
  	File SDCard;
	
	//Digital connection pin for SD card. 
	#define SD_PIN 10
};

#endif

