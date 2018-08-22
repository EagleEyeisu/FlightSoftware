/**
 * Keypad.cpp holds all functions related to the keypad. 
 */

#include <Arduino.h>
#include <Keypad.h>
#include "Globals.h"
#include "Data.h"
#include "keyP.h"
#include "Radio.h"


/**
 * Constructor used to reference all other variables & functions.
 */
KEYPAD::KEYPAD()
{
  
}


/**
 * Watches for keypad interaction. If so, updates "pressedKey" to respective integer value.  
 */
void KEYPAD::check(char key)
{
  // Resets the inputted key. 
  pressedKey = 0;
  
  // True if press. False is null. 
  if(key != '\0')
  {
    // Blinks LED onboard of LoRa to signal keypad interaction. 
    Radio.blinkLED();
    // Converts inputted char into its ascii equivalent integer. 
    int asciiValue = key;
    // Checks if integer is within keypad tolerance. (If its a valid button signal)
    if(asciiValue >= 49 && asciiValue <= 57)
    {
      // New info is being read in. 
      Data.newData = Data.YES;
      // Converts current key character value to the respective integer value. Key of '1'-> int of 1.
      pressedKey = convertCharToInt(key);
      // Serial.println(key);
    }
  }
}


/**
 * Converts the character "key" -> ascii equivalent -> respective keypad integer. 
 *                    char '5'  ->      int 53      ->      5 int
 */
int KEYPAD::convertCharToInt(char key){

  // Conversion to Ascii.
  int ascii = key;

  // Ascii to matching keypad integer.
  // Keypad button 1. 
  if(ascii == 49)
  {
    return 1;
  }
  else if(ascii == 50)
  {
    return 2;
  }
  else if(ascii == 51)
  {
    return 3;
  }
  else if(ascii == 52)
  {
    return 4;
  }
  else if(ascii == 53)
  {
    return 5;
  }
  else if(ascii == 54)
  {
    return 6;
  }
  else if(ascii == 55)
  {
    return 7;
  }
  else if(ascii == 56)
  {
    return 8;
  }
  else if(ascii == 57)
  {
    return 9;
  }
}
