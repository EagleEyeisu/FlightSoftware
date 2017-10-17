/**
 * I2C.h declares and holds the variables and frunctions used for I2C.
 */


#ifndef I2C_h
#define I2C_h

#include <Arduino.h>

class I2C
{
  public:

  //Constructor
  I2C();

  //Reads in a individual byte from the I2C port.
  void receiveEvent(int howmany); 
  
	//Runs initialzation script for the I2C protocol.
  void initialize();
  
  //Neighbor Discovery Protocol to be sent via I2C CAN bus.
  String NDP;

  bool First8 = false;

  bool Last8 = false;
};

#endif

