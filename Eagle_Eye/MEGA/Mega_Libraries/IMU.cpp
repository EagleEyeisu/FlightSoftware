/*
  IMU.cpp handles all of Eagle Eye's IMU data.
  This include all orientation data including:
  gyroscope, accelerometer, and magnetometer.
*/


#include "IMU.h"


/**
 * Tests connection to IMU.
 */
void IMU::IMU_Initialize()
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
	lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
	lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
	lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
	
}


/**
 * Responsible relaying to the LoRa that the craft is in the correct orientation to detach.
 */
void IMU::IMU_Manager()
{
	//Turns true when the LoRa has sent signal '8' to start the dropping process. 
	if (Detach_Request) {
		
		//Checks to see if chute is enabled and the craft is in the correct orientation, or
		//   if the timeout has occured with the chute being enabled..
		if ((ParachuteRedZone() && chute_enable) || (chute_enable && timeout >= 20)) {
			
			//Time out after 20 cycles. This is temporary, only looking to see if it actually is the correct
			//   orientation when ready for drop.
			if (timeout >= 20) {
				
				//Updates Mega's Event to log the detachment GO ahead.
				data.ME = 9;
				Serial.println("Timed out. LAUNCH");
			}
			
			//Sends detach GO Ahead to LoRa.
			Send_I2C(9);
			Serial.println("Sent Detach Signal");
			
			//Resets the variables.
			Detach_Request = false;
			timeout = 0;
			
			//Assigns the correct Mega Event number.
			data.ME = 9;
		}
		else {
			timeout++;
		}
	}
}


/**
 * Returns craft's current roll.
 */
float IMU::getRoll()
{
	//Updates object calibration to use most current IMU information.
	Adafruit_Simple_AHRS ahrs(&lsm.getAccel(), &lsm.getMag());
	
	//Checks for connection to IMU.
	if(ahrs.getOrientation(&orientation)){
		return (180 - abs(orientation.roll));
	}
}


/**
 * Returns craft's current pitch.
 */
float IMU::getPitch()
{
	//Updates object calibration to use most current IMU information.
	Adafruit_Simple_AHRS ahrs(&lsm.getAccel(), &lsm.getMag());
	
	//Checks for connection to IMU.
	if(ahrs.getOrientation(&orientation)){
		return (abs(orientation.pitch));
	}
}


/**
 * Returns craft's current yaw.
 */
float IMU::getYaw()
{
	//Updates object calibration to use most current IMU information.
	Adafruit_Simple_AHRS ahrs(&lsm.getAccel(), &lsm.getMag());
	
	//Checks for connection to IMU.
	if(ahrs.getOrientation(&orientation)){
		return (orientation.yaw);
	}
}


/**
 * Checks to see if the craft is in the right orientation to deploy.
 */
bool ParachuteRedZone(){
  return !(data.Roll >= 135 && data.Pitch <= 45);
}