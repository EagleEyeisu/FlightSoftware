/**
 * Save.h declares functions and varaibles used in Storage.cpp.
 */

#ifndef SAVE_h
#define SAVE_h

#include <Arduino.h>

class SAVE
{
    public:

    //Constructor
    SAVE();

    //Saves struct (current flight data), to SD card in correct format.
    void saveData();

    //Runs initialzation script for the SD card.
    void initialize();

    //Digital connection pin for SD card. 
    const byte SD_PIN = 10;
};

#endif

