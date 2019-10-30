/**
 * DATA.h is responsible for updating the programs main struct to the current most values.
 */ 

#ifndef Data_h
#define Data_h

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
  
class DATA
{
  	public:
	  // Constructor
	  DATA();
	  // Configures flags and LEDs.
    void initialize();
    // Manages the bootup process and status leds.
    void manager();
	  // Log the data to the sd card.
    void log_data();
	  // Parses passed in message by using commas as the identifiers.
	  float Parse(char message[], int objective);
	  // Responsible for pulling current sensor data from peripherals.
    void update_data();
    // Retrieves the flight controller's pressure value.
    float get_i2c_fltctrl_pressure(char buf[]);
    // Retrieves the flight controller's altitude value (calculated by pressure).
    float get_i2c_fltctrl_altitude(char buf[]);
    // Retrieves the flight controller's temperature value (external temp).
    float get_i2c_fltctrl_temp(char buf[]);
    // Retrieves the flight controller's roll value.
    float get_i2c_fltctrl_roll(char buf[]);
    // Retrieves the flight controller's pitch value.
    float get_i2c_fltctrl_pitch(char buf[]);
    // Retrieves the flight controller's yaw value.
    float get_i2c_fltctrl_yaw(char buf[]);
    // Retrieves the flight controller's target heading angle.
    float get_i2c_target_heading(char buf[]);
    // Retrieves the flight controller's current heading angle.
    float get_i2c_current_heading(char buf[]);
    // Retrieves the flight controller's craft state.
    float get_i2c_craft_state(char buf[]);
	  // Pulses external receive led.
    void blink_receive_led();
    // Pulses external sending led.
    void blink_send_led();
    // Pulses onboard error led.
    void blink_error_led();
	
	/*---------------------------------Variables---------------------------------*/

    /*----- Flight Controller variables -----*/

	// Altitude of the craft.
    float fltctrl_altitude = 0.0;
    // Roll value of the craft.
    float fltctrl_roll = 0.0;
    // Pitch value of the craft.
    float fltctrl_pitch = 0.0;
    // Yaw value of the craft.
    float fltctrl_yaw = 0.0;
    // External atmosphereic pressure.
    float fltctrl_pressure = 0.0;
    // External temperature of craft.
    float fltctrl_external_temperature = 0.0;
    // Current angle to the target.
	  float target_heading = 0.0;
	  // Current bearing for craft.
	  float craft_heading = 0.0;
	  // Movement state of the craft. This is what the craft is currently doing
	  // and not nessecarily what it wants to do.
	  float craft_state = 0.0;

    /*----- Radio Comms variables -----*/

	  // LoRa Event. This is assigned as needed throughout the program. Signals a specific event.
  	float craft_event = 0.0;
    // Set High for # (defined below) seconds on startup to tell other nodes in the system
    // that an unexpected power cycle has occurred and they need to clear 
    // their locally held variables.
    float node_reset = 0.0;
    // Number of seconds after startup that the node_reset will be held high.
    unsigned long startup_timer = 0;
    // Turns true after # seconds of running. Used to tell if the system started 
    // up correctly.
    bool system_boot_complete = false;
    // Power status of the LED outside of the box.
    bool external_led = false;
    // Timer to switch the external_led on/off at 1/2sec.
    unsigned long ext_led_timer = 0;
    // Timer to log data to the sd card every second.
    unsigned long sd_timer = 0;
    // DIGITAL LEDS.
    const byte OPERATIONAL_LED = 9;
    const byte RECEIVE_LED = A1;
    const byte SEND_LED = A2;
    const byte ERROR_LED = 13;
    // SD CARD.
    const byte SD_CS = 4;
};
#endif
