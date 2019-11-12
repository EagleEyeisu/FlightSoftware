/**
 * UART.h 
 */

#ifndef UART_h 
#define UART_h

#include <Arduino.h>

class UART
{
    public:
    // Constructor
    UART();
    // Reads in a individual byte from the UART port.
    void receiveEvent(int howMany);
    // Runs initialzation script for the UART protocol.
    void initialize();
    // Processes all outgoing packets.
    void manager();
    // Builds byte array to be sent to MEGA.
    void create_packet();
    // Sends byte array to MEGA.
    void send_packet();
    
    /*---------------------------------Variables---------------------------------*/
    
    // Inter-Intergrated Circuit (I^2C) packet.
    String uart_input_buffer;
    // Inter-Intergrated Circuit (I^2C) packet.
    String uart_packet = "";
    // Status of having a complete UART packet to parse.
    bool flag_complete_packet = false;
    // Selects between the Craft data(1), Target data(2), and network data(3).
    int uart_selector = 1;
    // Controls who has access to send over the UART line.
    bool uart_my_turn;
    // Makes sure the packet type is only set once per turn to maintain
    // the cyclical order.
    bool uart_packet_set;
    // Timer.
    unsigned long uart_timer = 0;
};
#endif
