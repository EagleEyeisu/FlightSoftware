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

  //Returns the difference between target and current heading and switches turn booleans accordingly
  float angleToTarget();

  //Returns difference between target and current altitudes and switches up/down booleans accordingly
  float checkAltitude();

  //
  float checkDistance();

  //Tolerance for heading angle
  float angleTolerance = 5;

  //Tolerance for difference in altitudes
  float altitudeTolerance = 200;

  //Tolerance for distance to target
  float distanceTolerance = 20;

  //Booleans to determine right/left turning action
  bool turnRight = false;
  bool turnLeft = false;

  //Booleans to determine upward/downward motion action
  bool moveUp = false;
  bool moveDown = false;

  //Booleans to determine forward/backward motion action
  bool moveForward = false;
  bool moveBackward = false;
	
};

#endif
