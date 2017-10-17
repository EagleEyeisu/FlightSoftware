/**
 * DATA.h is responsible for updating the programs main struct to the current most values.
 */

#ifndef DATA_h
#define DATA_h

#include <Arduino.h>
  
class DATA
{
  public:
  
	//Constructor
	DATA();
  
	//Parses passed in message by using commas as the identifiers.
	float Parse(char message[], int objective);
  
	//Responsible for pulling current sensor data from peripherals.
	void manager();
	
	
	
	//Stores all of Eagle Eye's current flight data.
	//   The difference between this struct and the one initalized in Radio.h is that this information
	//   is saves gathered/used/saved locally. The Radio.h struct holds all the network information
	//   being passed between crafts (nodes).
	struct Flight_Data {
	
		//Altitude of the craft gathered from GPS.
		float Altitude = 0.0;

    //Current distance to target in meters.
    float TargetDistance = 0.0;
	
		//Latitude of the craft gathered from GPS.
		float Latitude = 0.0;
		
		//Longitude of the craft gathered from GPS.
		float Longitude = 0.0;
		
		//Satellite Count of the craft gathered from the GPS.
		float SatCount = 0.0;

    //Speed in meters per second.
    float Speed = 0.0;
		
		//LoRa Event. This is assigned as needed throughout the program. Signals a specific event.
		float LE = 0.0;
		
		//Mega Event. This is assigned as needed throughout the program. Signals the Mega had a 
		//   specific event.
		float ME = 0.0;

    //Time of flight. Used for data capture in SD card.
    char Time[10];

		//Holds the crafts previous altitude.
		float altPrevious = 0.0;
	  
		//Holds the crafts previous latitude.
		float latPrevious = 0.0;
	  
		//Holds the crafts previous longitude.
		float lonPrevious = 0.0;

    //Holds the crafts previous target distance.
    float TDPrevious = 0.0;

	};
	struct Flight_Data Local;
  
  //Target Latitude for craft.
  float TargetLat = 42.8080;
  
  //Target Longitude for craft.
<<<<<<< HEAD
  float TARGET_LON = -92.2582;

  //Target Altitude for the craft.
  float TARGET_ALTITUDE = 10000.0;
=======
  float TargetLon = -92.2582;
>>>>>>> f4f0a061dbbaa25f9f2de38a20d201a60ac642bd
  
};
#endif
