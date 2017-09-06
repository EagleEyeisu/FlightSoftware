/**
 * Thermo.h holds objects related to the use of the thermocouple.
 */

 
#ifndef Thermo_h
#define Thermo_h


class Thermo
{
  public:
  
  	//Returns the external temp from the thermocouple.
	void getTempExt();
	
	//Runs initialzation script for the Thermocouple.
	void Thermo_Initialize();
	
	
	
	
	//Object used to pull and store the Thermocouple's information.
	Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);
	
	//Pins used to establish connection and pull information from the Thermocouple.
	#define MAXDO   32
	#define MAXCS   30
	#define MAXCLK  31

};

#endif

