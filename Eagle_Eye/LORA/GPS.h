/**
 * GPS.h holds objects related to the use of thermocouples.
 */

#ifndef GPS_h
#define GPS_h

#include <Arduino.h>

class GPS
{
  public:
  
	//Constructor
	GPS();
  
	//Runs initialzation script for the GPS.
	void Initialize();
		
	//Responsible for reading in a new and full NMEA sentence from the GPS module.
	void Manager(class Data_in, class GPS_in, class I2C_in, class Para_in, class Radio_in, class Save_in);

  //Returns the crafts NMEA fixation value.
  float getFixation();
 
  //Updates the current struct with the previous cycles values.
  void Revert_Struct();




  //Holds the passed in class objects to gain access to their references.
  class Data;
  //class GPS;
  class I2C;
  class Para;
  class Radio;
  class Save;
  
	//Character array that holds the GPS's GGA NMEA Sentence.
	static uint8_t NMEA[150];

};
#endif
