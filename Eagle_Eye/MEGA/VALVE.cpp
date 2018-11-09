/**
 * VALVE.cpp is repsonsible for Valve Opening and Closing onboard Eagle Eye.
 */

#include "VALVE.h"
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
void VALVE::initialize()
{
  pinMode(VALVE_CTRL,OUTPUT);
}


void VALVE::manager()
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
void VALVE::open_valve()
{
  digitalWrite(VALVE_CTRL, HIGH);
}

 /**
 * Used to Close the Valve
 */
void VALVE::close_valve()
{
  digitalWrite(VALVE_CTRL, LOW);
}
