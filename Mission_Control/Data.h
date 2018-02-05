/**
 * Data.h is responsible for updating the programs main struct to the current most values.
 */

#ifndef Data_h
#define Data_h

#include <Arduino.h>

class DATA
{
  public:
  
  //Constructor
  DATA();

  //Responsible for pulling current sensor data from peripherals.
  void displayInfo();

  
  
	//Parses passed in message by using commas as the identifiers.
	float Parse(char[] message, int objective);
 
};

#endif
