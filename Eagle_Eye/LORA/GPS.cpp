/**
 * GPS.cpp is repsonsible for capturing telemetry data.
 */


#include <Arduino.h>
#include "Data.h"
#include "GPS.h"
#include "SoftwareSerial.h"



/**
 * Constructor used to reference all other variables & functions.
 */
GPS::GPS()
{
  
}


/**
 * Returns the GPS Fixation of the craft.
 *    Fixation -> 6th Command
 */
float GPS::getFixation()
{
  return (Data.Parse(NMEA,6));
}


/**
 * Initialization of the GPS module.
 */
void GPS::Initialize()
{
  
  //Directs the GPS serial port to focus on a specific pair of GPIO's onboard the micro controller.
  SoftwareSerial ss(11,10);
  
	//Opens up a serial connection between the micro controller and
	//   the GPS breakout board at a certain baudrate.
	ss.begin(9600);

  Serial.println("GPS Online");
}


/**
 * Responsible for reading in a new and full NMEA sentence from the GPS module.
 */
void GPS::Manager(class Data_in, class GPS_in, class I2C_in, class Para_in, class Radio_in, class Save_in)
{
  
  Data = Data_in;
  //GPS = GPS_in;
  I2C = I2C_in;
  Para = Para_in;
  Radio = Radio_in;
  Save = Save_in;
  
  //Directs the GPS serial port to focus on a specific pair of GPIO's onboard the micro controller.
  SoftwareSerial ss(11,10);

  //Opens up a serial connection between the micro controller and
  //   the GPS breakout board at a certain baudrate.
  ss.begin(9600);
  
	//Iterator used below when going through the NEMA sentence.
	int i = 0;
	
	//Iterators through the entire character array that holds the NMEA sentence.
	for(i=0;i<150;i++){
		
		//Voids the previously held information to reduce memory corruption.
		NMEA[i] = ' ';
	}
	
	//Used to time the process of reading in data from the GPS below.
	unsigned long start = millis();
	
	//Temporary character array used to pull in the new GGA NMEA sentence.
	char Arr[150];
	
	//Resets the previously defined iterator to be used again below.
	i = 0;
	
	//Iterator used to filter the NMEA sentences that the GPS is sending to the
	//   micro controller. Eagle Eye wants the sentence starting with $GPGGA.
	int j = 0;
	
	//Used to signal the start of a new NMEA sentence.
	//   The dollar sign is the first character in the incoming array. -> $GPGGA
	int dollar_counter = 0;
	
	//Reads in one character from the GPS one millisecond at a time over the course 
	//   of 1000 milliseconds.
	do
	{
		//Checks to see if there is incoming data over the serial connection 
		//   established with the GPS module.
		while(ss.available()){
			
			//Reads in one character from the serial port.
			Arr[i] = ss.read();
			
			//Checks if the current character is a dollar sign which means its the beginning of
			//   a new array.
			if(Arr[i]=='$'){
				
				//Iterates the dollar sign counter by 1.
				dollar_counter++;
			}
			
			//Records the following NMEA sentence until the start of a new NMEA sentence is detected.
			if(dollar_counter==1){
				
				//Copies over the desired NEMA sentence from the incoming stream of data.
				NMEA[j] = Arr[i];
				
				//Increments the iterator used to iterator through the NMEA sentence.
				j++;
			}
			
			//Increments the iterator used in the overall collection of the data streaming from the GPS.
			i++;
		}
	
	//This occurs over the period of 1000 milliseconds to ensure that an entire, yet correct and complete NMEA
	//   sentence has been obtained.
	}while(millis() - start < 1000);
	
	//0 if NO fix, not 0 if GOOOD fix.
	float tempfix = getFixation();
	
	//Checks for the correct fix of the GPS.
	if(tempfix != 1){
		
		//Assigns the LoRa Event to reflect the GPS losing satellite fixation.
		Data.Local.LE = 3;
		
		//Reverts the data struct's main varaibles to prevent the crafts from thinking its at ground level,
		//   which could trigger errors such as premature deployment of the parachute.
		Revert_Struct();
	}
	else{
		
		//Updates previous values to current values after the craft has verified that the new values are
		//   of correct format and value.
		Data.AltPrevious = Data::Parse(NMEA,9);
		Data.LatPrevious = Data::Parse(NMEA,2);
		Data.LonPrevious = Data::Parse(NMEA,4);
	}
 
}


/**
 * Updates the current struct with the previous cycles values. Used incase of 
 *    GPS loss or sensor error.
 */
void GPS::Revert_Struct()
{
  //Reverts values to that of the previous cycle.
  Data.Local.Altitude = Data.AltPrevious;
  Data.Local.Latitude = Data.LatPrevious;
  Data.Local.Longitude = Data.LonPrevious;
}
