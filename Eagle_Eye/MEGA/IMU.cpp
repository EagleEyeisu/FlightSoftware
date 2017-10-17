/*
  IMU.cpp handles all of Eagle Eye's IMU data.
  This include all orientation data including:
  gyroscope, accelerometer, and magnetometer.
*/


#include "IMU.h"
#include "DATA.h"
#include "Globals.h"
#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Simple_AHRS.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <Math.h>


/**
 * Constructor used to reference all other variables & functions.
 */
IMU::IMU()
{
  angleToTarget();
  checkAltitude();
  checkDistance();
}


/**
 * Tests connection to IMU.
 */
void IMU::initialize()
{
	//If invalid connection, the program will stall and print an error message.
	if(!lsm.begin()){
		Serial.print("PROBLEM WITH 9DOF");
		while(1);
	}
	//Valid connection, program proceeds as planned.
	else{
		Serial.println("9Dof Online.");
	}
	
	//Sets specific calibration values. DO NOT CHANGE.
	lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_16G);
	lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
	lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
	
}


/**
 * Responsible relaying to the LoRa that the craft is in the correct orientation to detach.
 */
void IMU::manager()
{
	
}


/**
 * Returns craft's current roll.
 */
float IMU::getRoll()
{
	//Updates object calibration to use most current IMU information.
	Adafruit_Simple_AHRS ahrs(&lsm.getAccel(), &lsm.getMag());

  sensors_vec_t   orientation;
  
	//Checks for connection to IMU.
	if(ahrs.getOrientation(&orientation)){
		return (orientation.roll);
	}
}


/**
 * Returns craft's current pitch.
 */
float IMU::getPitch()
{
	//Updates object calibration to use most current IMU information.
	Adafruit_Simple_AHRS ahrs(&lsm.getAccel(), &lsm.getMag());

  sensors_vec_t   orientation;
  
	//Checks for connection to IMU.
	if(ahrs.getOrientation(&orientation)){
		return (orientation.pitch);
	}
}


/**
 * Returns craft's current yaw.
 */
float IMU::getYaw()
{
	//Updates object calibration to use most current IMU information.
	Adafruit_Simple_AHRS ahrs(&lsm.getAccel(), &lsm.getMag());

  sensors_vec_t   orientation;
  
	//Checks for connection to IMU.
	if(ahrs.getOrientation(&orientation)){
		return (orientation.heading);
	}
}

/**
 * Returns the difference between target and current heading and switches turn booleans accordingly
 */
float IMU::angleToTarget()
{
  float dlat = Data.TARGET_LAT - Data.Latitude;
  float dlon = Data.TARGET_LON - Data.Longitude;

  float target = Math.atan(dlat/-dlon)*(180/Math.M_PI);
  if(dlong > 0 && dlat < 0){
    target += 180;
  }
  else if (dlon < 0 && dlat < 0){
    target -= 180;
  }

  float angle = target - getYaw();
  if(angle > 180){
    angle -= 360;
  }
  else if(angle < -180){
    angle += 360;
  }

  if(angle > angleTolerance){
    turnRight = true;
  }
  else if(angle < -angleTolerance){
    turnLeft = true;
  }
  else{
    turnRight = false;
    turnLeft = false;
  }

  return angle;
}

/**
 * Returns difference between target and current altitudes and switches up/down booleans accordingly
 */
float IMU::checkAltitude()
{
  float diffAlt = Data.TARGET_ALTITUDE - Data.Altitude;

  if(diffAlt > altitudeTolerance){
    moveUp = true;
  }
  else if(diffAlt < -altitudeTolerance){
    moveDown = true;
  }
  else{
    moveUp = false;
    moveDown = false;
  }

  return diffAlt;
}

/**
 * 
 */
float IMU::checkDistance()
{
  if(Data.TargetDistance > distanceTolerance){
     moveForward = true;
  }else{
     moveForward = false;
  }
}

}
