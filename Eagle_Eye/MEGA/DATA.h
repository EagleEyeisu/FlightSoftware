/**
 * DATA.h is responsible for updating the programs main struct to the current most values.
 */

#ifndef DATA_H
#define DATA_H

#include <Arduino.h>

class DATA
{
  public:
	
	// Constuctor
	DATA();
 
	// Parses passed in message by using commas as the identifiers.
  float Parse(String message, int objective);
  
  // Responsible for pulling current sensor data from peripherals.
	void updateData();

  // Prints all info to the screen in the organized fasion. 
  void toScreen();
	
	// Reads in external pressure and calculates altitude.
	float getAltitude(float _Pressure);

  // Reads in the altitude from the CAN Network.
  float getGPSAltitude();

  // Reads in the latitude from the CAN Network.
  float getGPSLatitude();

  // Reads in the longitude from the CAN Network.
  float getGPSLongitude();

  // Reads in the taret altitude from the CAN Network.
  float getGPSTargetAlt();

  // Reads in the target latitude from the CAN Network.
  float getGPSTargetLat();

  // Reads in the taret longitude from the CAN Network.
	float getGPSTargetLon();
  
  // Reads in the taret distance from the CAN Network.
  float getGPSTargetDistance();

  // Reads in the target Distance from the CAN Network.
  float getGPSSpeed();

  // Reads in the time of the LoRa from the CAN Network.
  void getGPSTime();
  
	// Runs initialzation script for the Barometer.
	void initialize();
  
  
  
	// State of Data.
  // This enum is used in the UI display process. It will watch all the variables
  // and if any have changed compared to the last cycle, it will switch to yes. 
  // When it is in the YES state, the toScreen() method will print out the newly
  // accuried data to the serial monitor. The purpose of this is have a readable UI
  // without having it update every few milliseconds. (Potential future solution is
  // would be to use an external terminal such as SuperPuTTy)
  enum dataState {NO, YES};
  enum dataState newData = YES; // Initializes to yes to begin. Prints init UI.

    
	// Stores all of Eagle Eye's current flight data.
	// The difference between this struct and the one initalized in Radio.h is that this information
	// is saves gathered/used/saved locally. The Radio.h struct holds all the network information
	// being passed between crafts (nodes).
	struct Flight_Data
  {

    // INFORMATION IS LCOAL TO THE MEGA.
    
		// Altitude of the craft.
		float Altitude = 0.0;
    
    // Roll value of the craft.
    float Roll = 0.0;
    
    // Pitch value of the craft.
    float Pitch = 0.0;
    
    // Yaw value of the craft.
    float Yaw = 0.0;
    
    // External atmosphereic pressure.
    float Pressure = 0.0;
    
    // External temperature of craft.
    float TempExt = 0.0;
    
		// LoRa Event. This is assigned as needed throughout the program. Signals a specific event.
		float LE = 0.0;
		
		// Mega Event. This is assigned as needed throughout the program. Signals the Mega had a 
		// specific event.
		float ME = 0.0;


    // BELOW INFORMATION IS BROUGHT OVER FROM LORA.
   
    
    // Altitude of the craft. (Brought over from LoRa)
    float GPSAltitude = 0.0;

    // Latitude of the craft. (Brought over from LoRa)
    float Latitude = 0.0;
    
    // Longitude of the craft. (Brought over from LoRa)
    float Longitude = 0.0; 

    // Target Altitude (Brought over from LoRa)
    float GPSTargetAlt = 0.0;
    
    // Target Latitude (Brought over from LoRa)
    float GPSTargetLat = 0.0;
    
    // Target Longitude (Brought over from LoRa)
    float GPSTargetLon = 0.0;
    
    // Current distance to target in meters. (Brought over from LoRa)
    float GPSTargetDistance = 0.0;
    
    // Speed in meters per second. (Brought over from LoRa)
    float GPSSpeed = 0.0;
    
    // Time of flight. (Brought over from LoRa)
    char GPSTime[10];
  
	};
	struct Flight_Data Local;

};

#endif

