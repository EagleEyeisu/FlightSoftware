/**
 * Thermo.h holds objects related to the use of the thermocouple.
 */

 
#ifndef THERMO_h
#define THERMO_h

#include <Arduino.h>

class THERMO
{
  public:
  
  //Constructor
  THERMO();
  
  //Returns the external temp from the thermocouple.
	float getTempExt();

};

#endif

