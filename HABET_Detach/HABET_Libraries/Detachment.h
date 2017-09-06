/**
 * Save.h declares functions and varaibles used in Storage.cpp.
 */

#ifndef Detachment_h
#define Detachment_h

class Detachment
{
  public:
  
	//Runs initialzation script for the servo.
	void Servo_Initialize();
	
	//Opens the servo causing the craft to drop from HABET. 
	void Servo_Manager();

	
	

	//Object used to control the position and attributes of onboard servo.
	Servo Detachment;
};

#endif

