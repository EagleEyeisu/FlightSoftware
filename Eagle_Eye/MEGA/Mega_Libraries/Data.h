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
	
	//Reads in external pressure and calculates altitude.
	float getAltitude(float _Pressure);
	
	//Runs initialzation script for the Barometer.
	void Pressure_Initialize();


	
	
	//Object used to communicate and pull information from the BMP085. (Pressure Sensor)
	Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
	
	//Stores all of Eagle Eye's current flight data. (LE = LoRa Event, ME = Mega Event)
    struct flight_data {
		float Altitude;
		float TempExt;
		float Pressure;
		float Roll;
		float Pitch;
		float Yaw;
		int LE = 0;
		int ME = 0;
	};
	struct flight_data data;
	
	//Object used to store the store the data pulled from the BMP085.
	sensors_event_t event;

};

#endif

