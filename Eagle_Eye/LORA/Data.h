/**
 * Data.h is responsible for updating the programs main struct to the current most values.
 */

#ifndef Data_h
#define Data_h

#include <Arduino.h>
  
class Data
{
  public:
  
	//Constructor
	Data();
  
  //Parses passed in message by using commas as the identifiers.
  static float Parse(char message[], int objective);
  
	//Returns the crafts Altitude from NMEA.
  float getAltitude();
  
  //Returns the crafts NMEA fixation value.
  float getFixation();
  
  //Returns the crafts Latitude from NMEA.
  float getLatitude();
  
  //Returns the crafts Longitude from NMEA.
  float getLongitude();
    
  //Returns the number of satellites triangulating information to the GPS module.
  float getSatCount();
  
	//Responsible for pulling current sensor data from peripherals.
	void Manager(class Data_in, class GPS_in, class I2C_in, class Para_in, class Radio_in, class Save_in);
  
	

  //Holds the passed in class objects to gain access to their references.
  //class Data;
  class GPS;
  class I2C;
  class Para;
  class Radio;
  class Save;
  
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

	};
	static struct Flight_Data Local;
  
  //Holds the crafts previous altitude.
  static float AltPrevious = 0.0;
  
  //Holds the crafts previous latitude.
  static float LatPrevious = 0.0;
  
  //Holds the crafts previous longitude.
  static float LonPrevious = 0.0;
  
	//This timer is used for syncing the mesh network. Every craft as 5 seconds to talk. 
	//   When this timer is at 0, that means the craft just broadcasted its data. When this
	//   this timer hits 15, it resets to 0. This insures that the 2 other nodes have their 5
	//   seconds to talk. 3nodes * 5seconds = 15seconds for a total netowrk revolution. 
	unsigned long start = 0;
  
};
#endif
