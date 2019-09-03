/**
 * Thermo.h holds objects related to the use of the thermocouple.
 */

 
#ifndef THERMO_h
#define THERMO_h

#include <Arduino.h>

class THERMO
{
    public:
    // Constructor
    THERMO();

    /*---------------------------------Variables---------------------------------*/

    // Returns the external temp from the thermocouple.
    float get_external_temperature();
};

#endif

