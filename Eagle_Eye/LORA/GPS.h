/**
 * GPS.h holds objects related to the use of thermocouples.
 */

#ifndef GPS_h
#define GPS_h

#include <Arduino.h>

class GPS
{
  public:
	
	// Constructor
	GPS();

	// Returns the crafts NMEA fixation value.
	float getFixation();
	
	// Responsible for reading in a new and full NMEA sentence from the GPS module.
	void manager();

	// Using the current most gps data, calculates the distance to the target destination.
	// Returns a float in meters.
	float DistanceToTarget();
 
	// Updates the current struct with the previous cycles values.
	void revertStruct();
	
};
#endif
