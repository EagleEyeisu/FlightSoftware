/**
 * I2C.h declares and holds the variables and frunctions used for I2C.
 */


#ifndef I2C_h
#define I2C_h

#include <Arduino.h>

class I2C
{
    public:
    // Constructor.
    I2C();
    // Reads in a individual byte from the I2C port.
    void receiveEvent(int howMany); 
    // Runs initialzation script for the I2C protocol.
    void initialize();


    /*---------------------------------Variables---------------------------------*/

    // Inter-Intergrated Circuit (I^2C) packet.
    String i2c_buffer;
    // Status of having a complete i2c packet to parse.
    bool flag_complete_packet = false;
};

#endif
