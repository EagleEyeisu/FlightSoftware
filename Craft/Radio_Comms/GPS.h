/**
 * GPS.h holds objects related to the use of positioning.
 */

#ifndef GPS_h
#define GPS_h

#include <Arduino.h>

class GPS
{
  public:
	
	// Constructor
	GPS();
	// Responsible for reading in a new and full NMEA sentence from the GPS module.
	void manager();
	// Pulls current data from GPS module.
	void retrieve_gps_data();
	// Checks for a valid gps satellite fix.
	bool fixation_monitor();
	// Takes the new gps data and assigns it to the correct variables.
	void store_data();
	// Updates the current struct with the previous cycles values. (Prevents 0's due to gps fix loss)
	void revert_gps_data();
	// Using the current most gps data, calculates the distance to the target destination.
	// Returns a float in meters.
	float calculate_target_distance();

	/*---------------------------------Variables---------------------------------*/

	
	// Altitude of the craft gathered from GPS.
	float craft_altitude  = 0.0;
	// Latitude of the craft gathered from GPS.
	float craft_latitude = 0.0;
	// Longitude of the craft gathered from GPS.
	float craft_longitude = 0.0;
	// Satellite Count of the craft gathered from the GPS.
	float craft_satillite_count = 0.0;
	// Speed in meters per second.
	float craft_speed = 0.0;
	// Time of flight. Used for data capture in SD card.
	char craft_gps_time[10];
	// Target Altitude for the craft.
	float craft_target_altitude = 10000.0;
	// Target Latitude for craft.
	float craft_target_latitude = 0.0;
	// Target Longitude for craft.
	float craft_target_longitude = 0.0;
	// Current distance to target in meters.
	float craft_distance = 0.0;
	// Holds the crafts previous target distance.
	float previous_distance = 0.0;
	// Holds the crafts previous altitude.
	float previous_altitude = 0.0;
	// Holds the crafts previous latitude.
	float previous_latitude = 0.0;
	// Holds the crafts previous longitude.
	float previous_longitude = 0.0;
	// GPS satellite fixation status for the craft.
	bool fix_status = false;
	// Used to only read from the GPS every # seconds.
	unsigned long gps_block = 0.0;
};
#endif
