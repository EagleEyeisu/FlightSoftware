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
    // Processes all outgoing packets.
    void manager();
    // Builds byte array to be sent to MEGA.
    void create_packet();
    // Monitors for and responds to deadlock occurances.
    void deadlock_monitor();
    // Sends byte array to MEGA.
    void send_packet();

    /*---------------------------------Variables---------------------------------*/

    // Inter-Intergrated Circuit (I^2C) packet.
    String i2c_input_buffer;
    // Inter-Intergrated Circuit (I^2C) packet.
    String i2c_packet = "";
    // Status of having a complete i2c packet to parse.
    bool flag_complete_packet = false;
    // Selects between the (INSERT DESCRIPTORS HERE).
    int i2c_selector = 1;
    // Controls who has access to send over the i2c line.
    bool i2c_my_turn;
    // Makes sure the packet type is only set once per turn to maintain
    // the cyclical order.
    bool i2c_packet_set = false;
    // Timer.
    unsigned long i2c_timer = 0;
    // Counter variable used in monitoring for deadlock occurances.
    int i2c_deadlock_counter = 0;
};

#endif
