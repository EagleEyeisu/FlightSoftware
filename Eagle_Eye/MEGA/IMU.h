/**
 * IMU.h library file.
 */

#ifndef IMU_h
#define IMU_h

#include <Arduino.h>

class IMU
{
  public:

  // Constructor
  IMU();
  
  // Runs initialzation script for the IMU.
  void initialize();

  // Responsible relaying to the LoRa that the craft is in the correct orientation to detach.
  void manager();
  
  // Returns the crafts current Roll angle.
  float getRoll();
  
  // Returns the current Pitch angle.
  float getPitch();
  
  // Returns the current Yaw angle.
  float getYaw();

  // Returns the difference between target and current heading and switches turn booleans accordingly
  void angleToTarget();

  // Returns difference between target and current altitudes and switches up/down booleans accordingly
  void checkAltitude();

  // Compares current altitude against target altitude. Sets corresponding booleans.
  void checkDistance();

  // Current angle to the target.
  float ATT = 0.0;

  // Current bearing for craft
  float bearing = 0.0;
  
  // Tolerance for heading angle
  float angleTolerance = 10.0;

  // Tolerance for difference in altitudes
  float altitudeTolerance = 200.0;

  // Tolerance for distance to target
  float distanceTolerance = 20.0;

  // Booleans to determine right/left turning action
  bool turnRight = false;
  bool turnLeft = false;

  // Booleans to determine upward/downward motion action
  bool moveUp = false;
  bool moveDown = false;

  // Booleans to determine forward/backward motion action
  bool moveForward = true;  
  bool moveBackward = false;
	
};

#endif
