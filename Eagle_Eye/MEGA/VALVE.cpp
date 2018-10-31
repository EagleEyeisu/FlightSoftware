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


VALVE::initialize()
{
  pinMode(VAVLE_CTRL,OUTPUT);
}
