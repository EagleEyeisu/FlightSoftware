/**
 * VALVE.h holds objects related to the use of the Valve.
 */

 
#ifndef VALVE_h
#define VALVE_h

#include <Arduino.h>

class VALVE
{
  public:
  
  // Constructor
  VALVE();
  // Initialization function for the value.
  void initialize();
  // This Opens the Valve.
  void open_valve();
  // This Closes the valve.
  void close_valve();
  // Manages Valve Logic.
  void manager();
  /*-----------------------------------------------------------------------------*/
  // Valve Control
  #define VALVE_CTRL 20
  // This is going to control whether we open or close the valve.
  // 0-close.
  // 1-open.
  int valve_switch = 0;
  
};

#endif
