/**
 * Para.h holds predefinitions for all methods and declares used variables.
 */

#ifndef Para_h
#define Para_h

#include <Arduino.h>

class Para
{
  public:
	
	//Constructor
	Para();
	
	//Runs initialzation script for the Parachute.
	void Initialize();
		
	//Manages all parachute functions. Monitors and reacts accordingly based on crafts situation.
	void Manager(class Data_in, class GPS_in, class I2C_in, class Para_in, class Radio_in, class Save_in);
		
		
		

  //Holds the passed in class objects to gain access to their references.
  class Data;
  class GPS;
  class I2C;
  //class Para;
  class Radio;
  class Save;
  
	//Digitial pin used to connect the the Parachute's Relay.
	#define RELAY1  12
		
	//Status of chute deployment.
	bool chute_deploy = false;

	//Status of chute readiness.
	bool chute_enable = false;
		
	//Triggered by recieving a certain radio signal ('EDEPLOY' or 'FINAL') from Mission Control.
	//   When turned true, instantaneously (no checks for chute enablement or altitude) deploys the parachute. 
	bool EMERGENCY_DEPLOY = false;
		
	//Variable to hold the parachute's ENABLEMENT height.
	//   9144 m == 30,000 feet
	//   7620 m == 25,000 feet
	//   Not sure what this is -> (249.0)
	float PARACHUTE_ARM_HEIGHT = 144;

	//Variable to hold the parachute's DEPLOYMENT height.
	//   Must be less that ENABLEMENT height
	//   6096 m == 20,000 feet
	//   Not sure what this is -> (247.0)
	float PARACHUTE_DEPLOY_HEIGHT = 96;

	//Counter used to timeout after 4 seconds. Used in the parachute ENABLEMENT process.
	int saftey_counter = 0;

};

#endif
