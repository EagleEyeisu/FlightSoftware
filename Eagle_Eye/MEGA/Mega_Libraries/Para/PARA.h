/**
 * Para.h holds predefinitions for all methods and declares used variables.
 */

#ifndef Para_h
#define Para_h

class Para
{
  public:
	//Runs initialzation script for the Parachute.
	void Para_Initialize();
	
	//Manages all parachute functions. Monitors and reacts accordingly based on crafts situation.
    void Para_Manager();
	
	//Helper method to Deployment_Orientation(). Literally checks the orientation and returns 
	//   true of false.
	bool RedZone();
	
	
	
	//Digitial pin used to connect the the Parachute's Relay.
	#define RELAY1  28
	
	//Status of chute deployment.
    bool chute_deploy = false;
	
	//Status of chute readiness.
	bool chute_enable = false;
	
	//Variable to hold the parachute's ENABLEMENT height.
	//   9144 m == 30,000 feet
	//   7620 m == 25,000 feet
	//   Not sure what this is -> (249.0)
	float PARACHUTE_ARM_HEIGHT = 144;
	
	//Variable to hold the parachute's DEPLOYMENT height.
	//   Must be less that ENABLEMENT height
	//   6096m == 20,000 feet
	//   Not sure what this is -> (247.0)
	float PARACHUTE_DEPLOY_HEIGHT = 96;
	
	//Counter used to timeout after 4 seconds. Used in the parachute ENABLEMENT process.
	int saftey_counter = 0;
};

#endif

