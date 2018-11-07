/**
 * VALVE.cpp is repsonsible for Valve Opening and Closing onboard Eagle Eye.
 */

#include "Globals.h"
#include <Arduino.h>
 

 /**
 * Constructor used to reference all other variables & functions.
 */
VALVE::VALVE()
{
 
}


 /**
 * Setup The GPIO Pin for the Valve.
 */
VALVE::initialize()
{
  pinMode(VALVE_CTRL,OUTPUT);
}


VALVE::manager()
{
  if(valve_switch == 1)
  {
    open_valve(); 
  }
  else 
  {
    close_valve();
    
  }
}

 /**
 * Used to Open the Valve.
 */
VALVE::open_valve()
{
  digitalWrite(VALVE_CTRL, HIGH);
}

 /**
 * Used to Close the Valve
 */
VALVE::close_valve()
{
  digitalWrite(VALVE_CTRL, LOW);
}
