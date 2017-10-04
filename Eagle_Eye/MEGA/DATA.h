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



  
	
	//Stores all of Eagle Eye's current flight data.
	//   The difference between this struct and the one initalized in Radio.h is that this information
	//   is saves gathered/used/saved locally. The Radio.h struct holds all the network information
	//   being passed between crafts (nodes).
	struct Flight_Data {
	
		//Altitude of the craft.
		float Altitude = 0.0;

    //Altitude of the craft. (from LoRa)
    float GPSAltitude = 0.0;
	
		//Latitude of the craft.
		float Latitude = 0.0;
		
		//Longitude of the craft.
		float Longitude = 0.0;

    //Roll value of teh craft.
    float Roll = 0.0;

    //Pitch value of the craft.
    float Pitch = 0.0;
    
    //Yaw value of the craft.
    float Yaw = 0.0;

    //External atmosphereic pressure.
    float Pressure = 0.0;

    //External temperature of craft.
    float TempExt = 0.0;
    
		//LoRa Event. This is assigned as needed throughout the program. Signals a specific event.
		float LE = 0.0;
		
		//Mega Event. This is assigned as needed throughout the program. Signals the Mega had a 
		//   specific event.
		float ME = 0.0;

    //Speed in meters per second. (Brought over from LoRa)
    float Speed = 0.0;
    
    //Current distance to target in meters. (Brought over from LoRa)
    float TargetDistance = 0.0;

    //Time of flight. Used for data capture in SD card. (Brought over from LoRa)
    char Time[10];
    

	};
	struct Flight_Data Local;

};

#endif

