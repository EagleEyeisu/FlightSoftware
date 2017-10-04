/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#ifndef IMU_h
#define IMU_h

#include <Arduino.h>

class IMU
{
  public:

  //Constructor
  IMU();
  
	//Runs initialzation script for the IMU.
	void initialize();
	
	//Responsible relaying to the LoRa that the craft is in the correct orientation to detach.
  void manager();
	
	//Returns the crafts current Roll angle.
	float getRoll();
	
	//Returns the current Pitch angle.
	float getPitch();
	
	//Returns the current Yaw angle.
	float getYaw();
	
};

#endif
