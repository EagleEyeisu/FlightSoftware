/**
 * I2C.h declares and holds the variables and frunctions used for I2C.
 */


#ifndef I2C_h
#define I2C_h

#include <Arduino.h>

class I2C
{
    public:

    // Constructor
    I2C();

    // Reads in a individual byte from the I2C port.
    void receiveEvent(int howMany); 

    // Runs initialzation script for the I2C protocol.
    void initialize();

    /*-----------------------------------------------------------------------------*/

    // Inter-Intergrated Circuit (I^2C) packet.
    String i2c_buffer;

    // Status for reception of the first 32 bits via I2C protocol.
    bool first_32 = false;

    // Status for reception of the second 32 bits via the I2C protocol.
    bool second_32 = false;

    // Status for reception of the third 32 bits via the I2C protocol.
    bool third_32 = false;

    // Status of having a complete i2c packet to parse.
    bool complete_packet_flag = false;

    // Turns true during validation. If true, throw away i2c input for that cycle.
    bool packet_validation = false;

    // Forces the i2c parsing to alternate between direction and throttle.
    int i2c_alternate = 0;
};

#endif
