/**
 * GPS.h holds objects related to the use of thermocouples.
 */

#ifndef GPS_h
#define GPS_h

class GPS
{
  public:
  
  	//Returns the crafts Altitude from GPS.
	float getAltitude();
	
	//Returns the crafts GPS fixation value.
	float getFixation();
	
	//Returns the crafts Latitude from GPS.
	float getLatitude();
	
	//Returns the crafts Longitude from GPS.
	float getLongitude();
	
	//Returns the number of satellites triangulating information to the GPS module.
	float getSatCount();
	
	//Runs initialzation script for the GPS.
	void GPS_Initialize();
	
	//Responsible for reading in a new and full NMEA sentence from the GPS module.
	void GPS_Manager();
	
	
	
	//Character array that holds the GPS's GGA NMEA Sentence.
	char NMEA_Sentence[150];
	
	//Directs the GPS serial port to focus on a specific pair of GPIO's onboard the micro controller.
	SoftwareSerial ss(11,10);
	
};

#endif