/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#ifndef IMU_h
#define IMU_h

class IMU
{
  public:
	//Runs initialzation script for the IMU.
	void IMU_Initialize();
	
	//Responsible relaying to the LoRa that the craft is in the correct orientation to detach.
    void IMU_Manager();
	
	//Returns the crafts current Roll angle.
	float getRoll();
	
	//Returns the current Pitch angle.
	float getPitch();
	
	//Returns the current Yaw angle.
	float getYaw();
	
	//Returns true if the craft is in an upright position to deploy.
	bool ParachuteRedZone();
	
	
	
	
	//Object used to pull and store information from the IMU.
	//   Use I2C, ID #1000
	Adafruit_LSM9DS0 lsm(1000);
	
	//Specific calibration settings for the lsm oject created above.
	Adafruit_Simple_AHRS ahrs(&lsm.getAccel(), &lsm.getMag());
	
	//Sensor object used to pull and store information from the IMU.
	sensors_vec_t orientation;
	
	//Used as a timer check for the IMU_Manager() method. 
    int timeout = 0;
	
};

#endif