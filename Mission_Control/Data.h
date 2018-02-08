/**
 * Data.h is responsible for updating the programs main struct to the current most values.
 */

#ifndef DATA_h
#define DATA_h

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

  //false = Direct Line of Commuication to Craft. (Eagle Eye Blimp)
  //   true = Receiving information through another craft. Not direct from Eagle Eye.
  //
  //   THIS WILL NOT BE IMPLEMENTED UNTIL THIRD NODE IS BROUGHT ONLINE. PLACEHOLDER.
  //
  bool flyByWire = false;
  
  
  
};

#endif
