/*
  IMU.cpp handles all of Eagle Eye's IMU data.
  This include all orientation data including:
  gyroscope, accelerometer, and magnetometer.
  This file also holds values used in the 
  calculation of the crafts orientatin.
*/


#include "IMU.h"
#include "DATA.h"
#include "MOTOR.h"
#include "Globals.h"
#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Simple_AHRS.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <math.h>


/**
 * Constructor used to reference all other variables & functions.
 */
IMU::IMU()
{
  
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
  //Calculates the angle between the crafts current heading and the target.
  angleToTarget();

  //Checks the difference between where we want to be and where we actual are in terms of altitude.
  checkAltitude();

  //Calculates the distance in meters from the craft to the target location.
  checkDistance();
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
 * Sets certain booleans true depending on the crafts orientation.
 */
void IMU::angleToTarget()
{
  float lat1 = Data.Local.Latitude;
  float lon1 = Data.Local.Longitude;
  float lat2 = Data.Local.GPSTargetLat;
  float lon2 = Data.Local.GPSTargetLon;

  float x = cos(lat1)*sin(lat2) - sin(lat1)*cos(lat2)*cos(lon2-lon1);
  float y = sin(lon2-lon1) * cos(lat2); 
  float bearing = atan2(y, x);

  float angle = Imu.getYaw() - bearing;
  if(angle > 180){
    angle -= 360;
  }
  else if(angle < -180){
    angle += 360;
  }

  if(angle < -angleTolerance){
    turnRight = false;
    turnLeft = true;
  }
  else if(angle > angleTolerance){
    turnLeft = false;
    turnRight = true;
  }
  else{
    turnRight = false;
    turnLeft = false;
  }
  ATT = angle;
}

/**
 * Sets certain booleans true depending on the crafts orientation.
 */
void IMU::checkAltitude()
{
  float diffAlt = Data.Local.GPSTargetAlt - Data.Local.GPSAltitude;

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
}

/**
 * 
 */
void IMU::checkDistance()
{
  if(Data.Local.GPSTargetDistance > distanceTolerance){
    moveForward = true;
  }
  else{
    moveForward = false;
  }
}
