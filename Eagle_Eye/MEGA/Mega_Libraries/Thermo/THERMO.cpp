/**
 * Thermo.cpp is repsonsible for temperature measurements onboard Eagle Eye.
 */


#include "Thermo.h"


/**
 * No real need for this, but why now keep it consistant.
 */
void Thermo::Thermo_Initialize()
{
	Serial.println("Thermocouple Amplifier Online.\n");
}
 
 
/**
 * Returns the current temperature of the thermocouple.
 */
float Thermo::getTempExt()
{
	return thermocouple.readCelsius();
}