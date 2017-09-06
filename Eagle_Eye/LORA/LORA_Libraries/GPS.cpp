/**
 * GPS.cpp is repsonsible for capturing telemetry data.
 */


#include "GPS.h"


/**
 * Returns the Altittude of the craft.
 *    Altitude -> 9th Command
 */
float GPS::getAltitude()
{
	return (Parse(NMEA_Sentence,9));
}


/**
 * Returns the GPS Fixation of the craft.
 *    Fixation -> 6th Command
 */
float GPS::getFixation()
{
	return (Parse(NEMA_Sentence,6));
}


/**
 * Returns the Latitude of the craft.
 *    Latitude -> 2nd Command
 */
float GPS::getLatitude()
{
	return (Parse(NMEA_Sentence,2));
}


/**
 * Returns the Longitude of the craft.
 *    Longitude -> 4th Command
 */
float GPS::getLongitude()
{
	return (Parse(NMEA_Sentence,4));
}


/**
 * Returns the Satellite Count of the craft.
 *    SatCount -> 7th Command
 */
float GPS::getSatCount()
{
	return (Parse(NMEA_Sentence,7));
}


/**
 * Initialization of the GPS module.
 */
void GPS::GPS_Initialize()
{
	//Opens up a serial connection between the micro controller and
	//   the GPS breakout board at a certain baudrate.
	ss.begin(9600);
	Serial.println("GPS Online");
}


/**
 * Responsible for reading in a new and full NMEA sentence from the GPS module.
 */
void GPS::GPS_Manager()
{
	//Iterator used below when going through the NEMA sentence.
	int i = 0;
	
	//Iterators through the entire character array that holds the NMEA sentence.
	for(i=0;i<150;i++){
		
		//Voids the previously held information to reduce memory corruption.
		NMEA_Sentence[i] = ' ';
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
				NMEA_Sentence[j] = Arr[i];
				
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
		data.LE = 3;
		
		//Reverts the data struct's main varaibles to prevent the crafts from thinking its at ground level,
		//   which could trigger errors such as premature deployment of the parachute.
		Revert_Strut();
	}
	else{
		
		//Updates previous values to current values after the craft has verified that the new values are
		//   of correct format and value.
		AltPrevious = data.Altitude;
		LatPrevious = data.Latitude;
		LonPrevious = data.Longitude;
		
	}
}