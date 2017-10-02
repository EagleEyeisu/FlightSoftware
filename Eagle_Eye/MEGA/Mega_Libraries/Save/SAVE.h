/**
 * Save.h declares functions and varaibles used in Storage.cpp. File EagleEyeData 
 *    replacd by File SDCard because its shorter.
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
	

	
	
	//Digital connection pin for SD card. 
	#define SD_PIN 53
	
	//File object used to access the SD card.
  	File SDCard;
	
};

#endif

