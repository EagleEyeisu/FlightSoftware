/**
 * Thermo.cpp is repsonsible for temperature measurements onboard Eagle Eye.
 */


#include "THERMO.h"
#include "Globals.h"
#include <Adafruit_MAX31855.h>
#include <Arduino.h>
 

 /**
 * Constructor used to reference all other variables & functions.
 */
THERMO::THERMO()
{
 
}


/**
 * Returns the current temperature of the thermocouple.
 */
float THERMO::get_external_temperature()
{
	return thermocouple.readCelsius();
}
