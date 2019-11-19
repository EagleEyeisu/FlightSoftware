/****LIBRARIES****/
//NON EAGLE EYE
#include <RH_RF95.h>
#include <Time.h>

//EAGLE EYE MADE
#include "Globals.h"
#include "Data.h"
#include "Radio.h"

/*****CONSTRUCTORS*****/
DATA Data;
RADIO Radio;

//Directs the radio object to focus on two specific ports.
RH_RF95 rf95(Radio.RFM95_CS, Radio.RFM95_INT);

/**
 * Method initializes the main hardware components.
 */
void setup()
{
    // Creates a serial communication line between the arduino
    // and the serial port found under 'Tools' -> 'Serial Monitor'
    Serial.begin(115200);
    // Initializes the Radio.
    Radio.initialize();
    // Bootup has happened. Set flags.
    //Data.node_reset = 1;
    //Data.system_boot_complete = false;
}

/**
 * MAIN PROGRAM CODE.
 */
void loop()
{
    //Verbosity message.
    //Serial.println("...");
    // Reads in serial port data if available.
    serial_input();
    // Monitors if the LoRa just reset and changes values accordingly.
    //system_boot();
    // Ensures the gui is connected prior to starting the microcontroller's tasks.
    if (Data.gui_connection)
    {
        // Updates GUI/user with situational info.
        Data.update_gui();
    }
    // Responsible for grabbing all of the craft's current information,
    // turning that data into an array that can be sent out via radio.
    // Also reads in incoming messages.
    Radio.manager();
}

/**
 * Flag management during and after boot process.
 */
void system_boot()
{
    // For the first # seconds of boot.
    if ((millis() - Data.startup_timer >= 3000) && !Data.system_boot_complete)
    {
        // System has now been operating for # seconds.
        Data.node_reset = 0;
        // Adjust flag.
        Data.system_boot_complete = true;
    }
}

/**
 * Reads in serial data.
 */
void serial_input()
{
    if (Serial.available())
    {
        String new_input = "";
        while (Serial.available())
        {
            char t = Serial.read();
            new_input += t;
        }
        // Checks for the python gui starting up and attemping to
        // establish serial connection to this microcontroller.
        if (new_input[0] == 'P' && Data.gui_connection == false)
        {
            // Responds to the gui with the microcontroller ID tag.
            Serial.write("mission_control");
            // Start timer for serial writes.
            Data.serial_timer = millis();
            // Updates connection status.
            Data.gui_connection = true;
        }
        else if (new_input[0] == '$' && Data.gui_connection == true)
        {
            Serial.println("forward detected");
            Radio.manual_direction = Data.Parse(1, new_input);
        }
    }
}
