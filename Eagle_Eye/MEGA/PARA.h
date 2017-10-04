/**
 * Para.h holds predefinitions for all methods and declares used variables.
 */

#ifndef PARA_h
#define PARA_h

#include <Arduino.h>

class PARA
{
  public:

  //Constructor
  PARA();
  
	//Runs initialzation script for the Parachute.
	void initialize();
	
	//Manages all parachute functions. Monitors and reacts accordingly based on crafts situation.
  void manager();


  
	
	//Digitial pin used to connect the the Parachute's Relay.
	const byte RELAY1 = 28;
	
	//Status of chute deployment.
  bool chuteDeploy = false;
	
	//Status of chute readiness.
	bool chuteEnable = false;
	
	//Variable to hold the parachute's ENABLEMENT height.
	//   9144 m == 30,000 feet
	//   7620 m == 25,000 feet
	//   Not sure what this is -> (249.0)
	int PARACHUTE_ARM_HEIGHT = 144;
	
	//Variable to hold the parachute's DEPLOYMENT height.
	//   Must be less that ENABLEMENT height
	//   6096m == 20,000 feet
	//   Not sure what this is -> (247.0)
	int PARACHUTE_DEPLOY_HEIGHT = 96;
	
	//Counter used to timeout after 4 seconds. Used in the parachute ENABLEMENT process.
	int safteyCounter = 0;
};

#endif

