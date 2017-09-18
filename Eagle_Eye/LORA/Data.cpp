/**
 * Data.cpp contains the struct that holds all the Crafts situational information.
 */


#include <Arduino.h>
#include "Data.h"
#include <stdlib.h>
#include "GPS.h"


/**
 * Constructor used to reference all other variables & functions.
 */
Data::Data()
{
  
}


/**
 * Returns a parsed section of the read in parameter. The parameter 'objective' represents 
 *    the comma's position from the beginning of the character array.
 */
float Data::Parse(char message[], int objective){

  //Example GGA NMEA sentence
  //
  //  $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
  
  //Example Eagle Eye transmission. 
  //
  //                  LORA                              HABET                         MISSION CONTROL                   CRAFT ID
  //  Time(ms),Altitude,Latitude,Longitude,LE, | Time(ms),Release Status, | Time(ms),Command Sent,Command Received, | Signal Origin
  
  //The number of commas that the program needs to pass before it started parsing the data.
  int GoalNumber = objective;
  
  //Used to iterate through the character arrays.
  int i = 0;
  int t = 0;
  
  
  //This iterator is used to pull the wanted part of the 'message' from the entire array.
  //   Used to gather information such as how long the new parsed section is.
  int tempIter = 0;
  
  //Counts the commas as the character array is iterated over.
  int Comma_Counter = 0;
  
  //This turns true when the correct number of commas has been achieved, which signals that the following 
  //   section is the part that the program wants to parse from the entire sentence.
  bool Goal = false;
  
  //Temporary string used to hold the newly parsed array.
  char tempArr[20];
  
  //Iterators over the entire array.
  for(i=0;i<120;i++){
    
    //Checks to see if the current iterator's position is a comma. 
    if(message[i] == ','){
      
      //If so, it iterators the comma counter by 1.
      Comma_Counter++;
    }
    //Checks to see if the desired amount of commas has been passed. 
    else if(Comma_Counter == GoalNumber){
      
      //Checks to see if the iterator's position is a comma, used to cause a stop in parsing.
      if(message[i] != ','){
        
        //Copies the message's character to the temporary array.
        tempArr[t] = message[i];
        
        //Iterator used to tell how long the temporary array is.
        tempIter++;
      }
    }
  }
  
  //Charater array used with a fitted length of the parsed section.
  char arr[tempIter];
  
  //Iterates through the temporary array copying over the info to the variable which will be returned.
  for(i=0;i<tempIter;i++){
    
    //Copying of the information between arrays.
    arr[i]=tempArr[i];
  }
  
  //Converts the final array to a float.
  float temp = atof(arr);
  
  //Returns the desired parsed section in number (float) form.
  return temp;
}


/**
 * Returns the Altittude of the craft.
 *    Altitude -> 9th Command
 */
float Data::getAltitude()
{
  return (Parse(GPS.NMEA,9));
}


/**
 * Returns the GPS Fixation of the craft.
 *    Fixation -> 6th Command
 */
float getFixation()
{
  return (Parse(GPS.NMEA,6));
}


/**
 * Returns the Latitude of the craft.
 *    Latitude -> 2nd Command
 */
float Data::getLatitude()
{
  return (Parse(GPS.NMEA,2));
}


/**
 * Returns the Longitude of the craft.
 *    Longitude -> 4th Command
 */
float Data::getLongitude()
{
  return (Parse(GPS.NMEA,4));
}


/**
 * Returns the Satellite Count of the craft.
 *    SatCount -> 7th Command
 */
float Data::getSatCount()
{
  return (Parse(GPS.NMEA,7));
}


/**
 * Updates the main struct for the LoRa. 
 */
void Data::Manager(class Data_in, class GPS_in, class I2C_in, class Para_in, class Radio_in, class Save_in)
{

  //Data = Data_in;
  GPS = GPS_in;
  I2C = I2C_in;
  Para = Para_in;
  Radio = Radio_in;
  Save = Save_in;
  
	//Updates all struct variables with the most current sensor data.
	Local.Altitude = getAltitude();
	Local.Latitude = getLatitude();
	Local.Longitude = getLongitude();
	Local.GPS_Fix = getFixation();
	Local.Satellite_Count = getSatCount();
	//Local.LE and Local.ME update on their own throughout the program & are reset to 0 after being saved.

  //Prints out data struct to the screen for debugging/following alone purposes.
  Serial.print("Altitude:   ");    Serial.print(Local.Altitude); Serial.println(" m");
  Serial.print("Latitude:   ");    Serial.println(Local.Latitude);
  Serial.print("Longitude:  ");    Serial.println(Local.Longitude);
  Serial.print("GPS Fix:    ");    Serial.println(Local.GPS_Fix);
  Serial.print("Sat Count:  ");  Serial.println(Local.Satellite_Count);
  Serial.print("LoRa Event: ");     Serial.println(Local.LE);
  Serial.print("Mega Event: ");     Serial.println(Local.ME);
  Serial.print("-------------------------------------------");
  Serial.println();
  
}
