/**
 * Keypad.h holds all declarations and varaibles related to the Keypad.cpp file.
 */

#ifndef KEYPAD_h
#define KEYPAD_h

#include <Arduino.h>

class KEYPAD
{
    public:

    // Constructor
    KEYPAD();

    // Checks for new key press. Records its value. 
    void check(char temp);

    // Converts char key to its button integer on the keypad. 
    int convertCharToInt(char key);


    // Current key being pressed down. If none, than = 0.
    int pressedKey;

    // Wires that represent each row.  
    byte rowPins[3] = {5, 6, 9};

    // Wires that represent each column.
    byte colPins[3] = {11, 12, 13};

    // Matrix layout of the keypad in use. 
    char keys[3][3] ={
                      {'1','2','3'},
                      {'4','5','6'},
                      {'7','8','9'},
                     };
};

#endif
