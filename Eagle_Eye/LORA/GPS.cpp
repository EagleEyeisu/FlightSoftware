/**
 * GPS.cpp is repsonsible for capturing telemetry data.
 */

 
#include <Arduino.h>
#include "GPS.h"
#include "DATA.h"
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
#include "Globals.h"
#include <math.h>


/**
 * Constructor used to reference all other variables & functions.
 */
GPS::GPS()
{
	
}


/**
 * Responsible for reading in a new and full NMEA sentence from the GPS module.
 */
void GPS::manager()
{
  
  //Directs the GPS serial port to focus on a specific pair of GPIO's onboard the micro controller.
  //   RX = 10      TX = 11
  SoftwareSerial ss(11,10);
  
  //Opens up a serial connection between the micro controller and
  //   the GPS breakout board at a certain baudrate.
  ss.begin(9600);

  //Reads in GPS data via serial port for 1 second.
  unsigned long start = millis();
  do 
  {
    //Reads in data while it is available.
    while (ss.available())

      //Stores the brought in data to the gps object.
      gps.encode(ss.read());

  //Count down timer for 1 second.
  } while (millis() - start < 1000);



  //Checks the correctness of the gps data. (Worthless if less than 5)
  if(gps.satellites.value() <  5){

    if(gps.satellites.value() == 0){
      Data.Local.LE = 3;
    }
    //If no fixation, to prevent all values setting to zero, 
    revertStruct();
  }
  else{
    
    //Updates all struct variables with the most current sensor data.
    sprintf(Data.Local.Time, "%02d:%02d:%02d ", gps.time.hour(), gps.time.minute(), gps.time.second());
    Data.Local.Altitude = gps.altitude.meters();
    Data.Local.Latitude = gps.location.lat();
    Data.Local.Longitude = gps.location.lng();
    Data.Local.SatCount = gps.satellites.value();
    Data.Local.Speed = gps.speed.mps(); 
    Data.Local.TargetDistance = DistanceToTarget();


    //Replaces the old backup values with the new values.
    Data.Local.altPrevious = Data.Local.Altitude;
    Data.Local.latPrevious = Data.Local.Latitude;
    Data.Local.lonPrevious = Data.Local.Longitude;
    Data.Local.TDPrevious = Data.Local.TargetDistance;
  }
}


/**
 * Calculates the distance to the GPS Target in meters.
 */
float GPS::DistanceToTarget(){
  
<<<<<<< HEAD
  float distance = (float)TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), Data.Local.TargetLat, Data.Local.TargetLon) / 100;

=======
  //float distance = (float)TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), Data.TARGET_LAT, Data.TARGET_LON) / 100;
  float distance = sqrt(pow((Data.Local.Latitude - Data.TargetLat),2)+pow((Data.Local.Longitude - Data.TargetLon),2));
  
>>>>>>> origin/test
  return distance;
}

/**
 * Updates the current struct with the previous cycles values. Used incase of 
 *    GPS loss or sensor error.
 */
void GPS::revertStruct()
{
  //Reverts values to that of the previous cycle.
  Data.Local.Altitude = Data.Local.altPrevious;
  Data.Local.Latitude = Data.Local.latPrevious;
  Data.Local.Longitude = Data.Local.lonPrevious;
  Data.Local.Speed = 0.0;
  Data.Local.TargetDistance = Data.Local.TDPrevious;
}
