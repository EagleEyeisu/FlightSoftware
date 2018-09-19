/**
 * DATA.h is responsible for updating the programs main struct to the current most values.
 */ 

#ifndef Data_h
#define Data_h

#include <Arduino.h>
  
class DATA
{
  public:
  
	// Constructor
	DATA();
  
	// Parses passed in message by using commas as the identifiers.
	float Parse(char message[], int objective);
	
	/*-----------------------------------------------------------------------------*/
  
	// Stores all of Eagle Eye's current flight data.
	// The difference between this struct and the one initalized in Radio.h is that this information
	// is saves gathered/used/saved locally. The Radio.h struct holds all the network information
	// being passed between crafts (nodes).
	struct Flight_Data 
	{
		// Altitude of the craft gathered from GPS.
		float current_altitude  = 0.0;
		
		// Latitude of the craft gathered from GPS.
		float current_latitude = 0.0;
		
		// Longitude of the craft gathered from GPS.
		float current_longitude = 0.0;
		
		// Satellite Count of the craft gathered from the GPS.
		float current_satillite_count = 0.0;

	    // Speed in meters per second.
	    float current_speed = 0.0;
		
		// LoRa Event. This is assigned as needed throughout the program. Signals a specific event.
		float current_event = 0.0;

	    // Time of flight. Used for data capture in SD card.
	    char current_gps_time[10];

	    // Target Altitude for the craft.
	    float current_target_altitude = 10000.0;

	   	// Target Latitude for craft.
	    float current_target_latitude = 0.0;
	  	
	    // Target Longitude for craft.
	    float current_target_longitude = 0.0;

	    // Current distance to target in meters.
	    float current_target_distance = 0.0;

		// Holds the crafts previous altitude.
		float previous_altitude = 0.0;
	  	
		// Holds the crafts previous latitude.
		float previous_latitude = 0.0;
	  	
		// Holds the crafts previous longitude.
		float previous_longitude = 0.0;
		
	    // Holds the crafts previous target distance.
	    float previous_target_distance = 0.0;
	};
	struct Flight_Data Local;
};
#endif

