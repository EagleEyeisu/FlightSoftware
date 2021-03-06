/**
 * IMU.cpp handles all of Eagle Eye's IMU data.
 * This include all orientation data including:
 * gyroscope, accelerometer, and magnetometer.
 * This file also holds values used in the 
 * calculation of the crafts orientatin.
 */


#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Simple_AHRS.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <math.h>

#include "IMU.h"
#include "GPS.h"
#include "DATA.h"
#include "MOTOR.h"
#include "Globals.h"


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
    // If invalid connection, the program will stall and print an error message.
    if(!lsm.begin())
    {
	    // If invalid connection, the program will stall and pulse the onbaord led.
        while (1)
        {
            Serial.println("IMU Init Error");
        }
    }
    // Sets specific calibration values. DO NOT CHANGE.
    lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_16G);
    lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
    lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
}


/**
 * Responsible relaying to the LoRa that the craft is in the correct orientation to detach.
 */
void IMU::manager()
{
    // Calculates the angle between the crafts current heading and the target.
    calculate_target_heading();
    // Calculates the distance in meters from the craft to the target location.
    check_distance_tolerance();
}


/**
 * Returns craft's current roll.
 */
float IMU::get_roll()
{
    // Updates object calibration to use most current IMU information.
    Adafruit_Simple_AHRS ahrs(&lsm.getAccel(), &lsm.getMag());
    // Creates data sample object.
    sensors_vec_t   data_sample;
    // Checks for connection to IMU.
    if(ahrs.getOrientation(&data_sample))
    {
	   return (data_sample.roll);
    }
}


/**
 * Returns craft's current pitch.
 */
float IMU::get_pitch()
{
    // Updates object calibration to use most current IMU information.
    Adafruit_Simple_AHRS ahrs(&lsm.getAccel(), &lsm.getMag());
    // Creates data sample object.
    sensors_vec_t   data_sample;
    // Checks for connection to IMU.
    if(ahrs.getOrientation(&data_sample))
    {
	   return (data_sample.pitch);
    }
}


/**
 * Returns craft's current yaw.
 */
float IMU::get_yaw()
{
    // Updates object calibration to use most current IMU information.
    Adafruit_Simple_AHRS ahrs(&lsm.getAccel(), &lsm.getMag());
    // Creates data sample object.
    sensors_vec_t   data_sample;
    // Checks for connection to IMU.
    if(ahrs.getOrientation(&data_sample))
    {
	   return (data_sample.heading);
    }
}

/**
 * Given the current heading and the lat/lng of the wanted destination,
 * this method calculates the angle needed to turn either left (0 to -180)
 * or right (0 to +180).
 */
void IMU::calculate_target_heading()
{
    // Pulls in data to be used in calculations.
    float lat1 = Gps.gps_latitude;
    float lon1 = Gps.gps_longitude;
    float lat2 = Gps.target_latitude;
    float lon2 = Gps.target_longitude;
    // Math. (Ask Christopher Johannsen if problem occurs)
    float x = cos(lat1)*sin(lat2) - sin(lat1)*cos(lat2)*cos(lon2-lon1);
    float y = sin(lon2-lon1) * cos(lat2); 
    craft_heading = atan2(y, x);
    // Normalize to 0-360.
    craft_heading = fmod((craft_heading + 360.0),360.0);
    // Calculates the target heading.
    float target_heading = craft_heading - Imu.get_yaw();
    if(target_heading > 180)
    {
        target_heading -= 360;
    }
    else if(target_heading < -180)
    {
        target_heading += 360;
    }
    if(target_heading < -target_heading_tolerance)
    {
        turn_right = false;
        turn_left = true;
    }
    else if(target_heading > target_heading_tolerance)
    {
        turn_left = false;
        turn_right = true;
    }
    else
    {
        turn_right = false;
        turn_left = false;
    }
    target_heading = target_heading;
}

/**
 * Checks the distance from the craft to the destination.
 */
void IMU::check_distance_tolerance()
{
    // Checks if distance is within tolerance.
    if(Gps.gps_distance > target_distance_tolerance)
    {
        // If not, move the craft forward.
        move_forward = true;
    }
    // If within tolerance.
    else
    {
        // Power down the craft.
        move_forward = false;
    }
}
