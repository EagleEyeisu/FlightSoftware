/**
 * DATA.h is responsible for updating the programs main struct to the current most values.
 */

#ifndef DATA_H
#define DATA_H

#include <Arduino.h>

class DATA
{
  public:
	
	//Constuctor
	DATA();
	
  	//Responsible for pulling current sensor data from peripherals.
	void manager();
	
	//Reads in external pressure and calculates altitude.
	float getAltitude(float _Pressure);
	
	//Runs initialzation script for the Barometer.
	void initialize();


	
	
	//Object used to communicate and pull information from the BMP085. (Pressure Sensor)
	Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
	
	//Stores all of Eagle Eye's current flight data.
	//   The difference between this struct and the one initalized in Radio.h is that this information
	//   is saves gathered/used/saved locally. The Radio.h struct holds all the network information
	//   being passed between crafts (nodes).
	struct Flight_Data {
	
		//Altitude of the craft gathered from GPS.
		float Altitude = 0.0;
	
		//Latitude of the craft gathered from GPS.
		float Latitude = 0.0;
		
		//Longitude of the craft gathered from GPS.
		float Longitude = 0.0;
		
		//GPS Fixation of the craft gathered from the GPS.
		float GPS_Fix = 0.0;
		
		//Satellite Count of the craft gathered from the GPS.
		float Satellite_Count = 0.0;
		
		//LoRa Event. This is assigned as needed throughout the program. Signals a specific event.
		float LE = 0.0;
		
		//Mega Event. This is assigned as needed throughout the program. Signals the Mega had a 
		//   specific event.
		float ME = 0.0;

		//Holds the crafts previous altitude.
		float altPrevious = 0.0;
	  
		//Holds the crafts previous latitude.
		float latPrevious = 0.0;
	  
		//Holds the crafts previous longitude.
		float lonPrevious = 0.0;

	};
	static struct Flight_Data Local;
	
	//Object used to store the store the data pulled from the BMP085.
	sensors_event_t event;

};

#endif

