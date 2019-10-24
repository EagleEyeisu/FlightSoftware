/**
 * Tests if physical and digital connections are valid. 
 * Prints out status to serial port. 
 */

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <SPI.h>
#include <SD.h>


// Creates an instance of the gps class from TinyGPSPlus.
TinyGPSPlus gps;
// Used to only read from the GPS every # seconds.
unsigned long gps_sample_timer = 0.0;
// Used to only read from the GPS every # seconds.
unsigned long gps_read_in_timer = 0.0;
// Holds the GPS time data.
char craft_gps_time[10];
// Used with SD card.
const int chipSelect = 4;

void setup()
{
    // Connection to laptop via usb. 
    Serial.begin(115200);
    while(!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB
    }
    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect))
    {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        while (1);
    }
    // Logs data to SD card.
    log_data(2);
}

/**
 * Samples data from the GPS and logs it to the onboard SD card. 
 */
void loop()
{
    // Samples GPS once per second.
    if (millis() - gps_sample_timer > 1000)
    {
        // Reset timer.
        gps_sample_timer = millis();
        // Pull new data from GPS.
        retrieve_gps_data();
        // Checks for GPS fix.
        if(gps.satellites.value() > 0)
        {
            // Prints out data via serial port.
            dispaly_data();
            // Logs data to SD card.
            log_data(1);
        }
        else
        {
            log_data(3);
        }
    }
}


/**
 * Pulls current data from GPS module.
 */
void retrieve_gps_data()
{
    // Opens up a serial connection between the micro controller and
    // the GPS breakout board at a certain baudrate.
    Serial1.begin(9600);
    // Reads in GPS data via serial port over a 1 second period.
    unsigned long gps_read_in_timer = millis();
    do
    {
        // Reads in data while it is available.
        while (Serial1.available())
        {
            // Stores the brought in data to the gps object.
            gps.encode(Serial1.read());
        }
    // Count down timer for 1 second.
    } while (millis() - gps_read_in_timer < 1000);
}


/**
 * Prints the recieved gps data to the serial port. Will print 0's if no fix.
 */
void dispaly_data()
{   
    // Puts all time variables into a single variable. (Easier to print)
    sprintf(craft_gps_time, "%02d:%02d:%02d ", gps.time.hour(), gps.time.minute(), gps.time.second());
    Serial.println("******************NEW PACKET******************");
    Serial.print("Time:      ");  Serial.println(craft_gps_time);
    Serial.print("Altitude:  ");  Serial.println(gps.altitude.meters());
    Serial.print("Latitude:  ");  Serial.println(gps.location.lat());
    Serial.print("Longitude: ");  Serial.println(gps.location.lng());
    Serial.print("Sat #:     ");  Serial.println(gps.satellites.value());
    Serial.print("Speed (m): ");  Serial.println(gps.speed.mps());
}


/**
 * Logs appropriate data to sd card.
 * 1 = gps
 * 2 = restart
 */
void log_data(int selector)
{
    // Check for temperature log. 
    // (Even though it should be 1, sometimes it 
    // comes through as 1.1 or 0.9 etc..)
    if(0.8 < selector && selector < 1.2)
    {
        // Opens the text file. Create one if it doesn't exist.
        File dataFile = SD.open("gps.txt", FILE_WRITE);
        // Checks for valid file.
        if(dataFile)
        {
            String log = "";
            log += "******************NEW PACKET******************\n";
            log += "Time:       ";  
            log += craft_gps_time;
            log += "\n";
            log += "Altitude:   ";  
            log += gps.altitude.meters();
            log += "\n";
            log += "Latitude:   ";  
            log += gps.location.lat();
            log += "\n";
            log += "Longitude:  ";  
            log += gps.location.lng();
            log += "\n";
            log += "Sat #:      ";  
            log += gps.satellites.value();
            log += "\n";
            log += "Speed(mps): ";
            log += gps.speed.mps();
            log += "\n";
            dataFile.println(log);
            dataFile.close();
            // Print to serial port.
            Serial.println(log);
        }
    }
    else if(1.8 < selector && selector < 2.2)
    {
        // Opens the text file. Create one if it doesn't exist.
        File dataFile = SD.open("gps.txt", FILE_WRITE);
        // Checks for valid file.
        if(dataFile)
        {
            String log = "";
            log += "------------------RESTARTING MICROCONTROLLER----------------------------";
            dataFile.println(log);
            dataFile.close();
            Serial.println(log);
        }
    }
    else if(2.8 < selector && selector < 3.2)
    {
        // Opens the text file. Create one if it doesn't exist.
        File dataFile = SD.open("gps.txt", FILE_WRITE);
        // Checks for valid file.
        if(dataFile)
        {
            String log = "";
            log += "------";
            dataFile.println(log);
            dataFile.close();
            Serial.println(log);
        }
    }
}
