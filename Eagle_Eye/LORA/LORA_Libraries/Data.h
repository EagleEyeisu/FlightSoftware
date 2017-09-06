/**
 * Data.h is responsible for updating the programs main struct to the current most values.
 */

#ifndef Data_h
#define Data_h

class Data
{
  public:
	
	//Responsible for pulling current sensor data from peripherals.
	void Data_Manager();
	
	//Parses passed in message by using commas as the identifiers.
	float Parse(char[] message, int objective);
	
	//Updates the current struct with the previous cycles values.
	void Revert_Struct();

	
	
	//Holds the crafts previous altitude.
	float AltPrevious = 0.0;
	
	//Holds the crafts previous latitude.
	float LatPrevious = 0.0;
	
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
		float Longitude 0.0;
		
		//GPS Fixation of the craft gathered from the GPS.
		int GPS_Fix = 0.0;
		
		//Satellite Count of the craft gathered from the GPS.
		int Satellite_Count = 0.0;
		
		//LoRa Event. This is assigned as needed throughout the program. Signals a specific event.
		int LE = 0.0;
		
		//Mega Event. This is assigned as needed throughout the program. Signals the Mega had a 
		//   specific event.
		int ME = 0.0;
	};
	struct Flight_Data Local;
	
	//Holds the crafts previous longitude.
	float LonPrevious = 0.0;
	
	//This timer is used for syncing the mesh network. Every craft as 5 seconds to talk. 
	//   When this timer is at 0, that means the craft just broadcasted its data. When this
	//   this timer hits 15, it resets to 0. This insures that the 2 other nodes have their 5
	//   seconds to talk. 3nodes * 5seconds = 15seconds for a total netowrk revolution. 
	unsigned long start = 0;
	
	//Timer object used for tracking overall operation time of the craft.
	time_t Time;

};

#endif