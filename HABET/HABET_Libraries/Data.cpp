/**
 * Data.cpp contains the struct that holds all the Crafts situational information.
 */


#include "Data.h"


/**
 * Returns a parsed section of the read in parameter. The parameter 'objective' represents 
 *    the comma's position from the beginning of the character array.
 */
float Data::Parse(char[] message, int objective){

	//Example GGA NMEA sentence
	//
	//  $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
	
	//Example Eagle Eye transmission. 
	//
	//                  LORA                              HABET                         MISSION CONTROL                   CRAFT ID
	//  Time(ms),Altitude,Latitude,Longitude,LE, | Time(ms),Release Status, | Time(ms),Command Sent,Command Received, | Signal Origin
	
	//The number of commas that the program needs to pass before it started parsing the data.
	int GoalNumber = objective;
	
	//Used to iterate through the character array..
	int i;
	
	//This iterator is used to pull the wanted part of the 'message' from the entire array.
	//   Used to gather information such as how long the new parsed section is.
	int tempIter = 0;
	
	//Counts the commas as the character array is iterated over.
	int Comma_Counter = 0;
	
	//This turns true when the correct number of commas has been achieved, which signals that the following 
	//   section is the part that the program wants to parse from the entire sentence.
	bool Goal = false
	
	//Temporary string used to hold the newly parsed array.
	String tempArr = "                   ";
	
	//Iterators over the entire array.
	for(i=0;i<120;i++){
		
		//Checks to see if the current iterator's position is a comma. 
		if(NMEA_Sentence[i] == ','){
			
			//If so, it iterators the comma counter by 1.
			Comma_Counter++;
		}
		//Checks to see if the desired amount of commas has been passed. 
		else if(Comma_Counter == GoalNumber){
			
			//Checks to see if the iterator's position is a comma, used to cause a stop in parsing.
			if(NMEA_Sentence[i] != ','){
				
				//Copies the message's character to the temporary array.
				tempArr[t] = NMEA_Sentence[i];
				
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
		arr[i]=temp[i];
	}
	
	//Converts the final array to a float.
	float temp = atof(arr);
	
	//Returns the desired parsed section in number (float) form.
	return temp;
}