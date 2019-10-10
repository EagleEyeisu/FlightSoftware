#include <Arduino.h>
#include <Servo.h> 
#include <SPI.h>
#include <SD.h>
#include <Adafruit_MAX31855.h>

/***** MOTOR VARIABLES *****/

// Specific amount to increment/decrement the PWM waveform between each pulse. 
// This ensures that each increment or decrement is suttle to not burn the motors out.
// Unit = microseconds
int increment_amount = 10;
// Delay time between consecutive pulses to the motors.
// Unit = milliseconds
#define INCREMENT_DELAY 320
// PWM pin that goes to the esc.
#define MOTOR_PIN 11
// Software object that will interact with the Electronic Speed Controller. (We treat this as the EDF itself)
Servo motor;
// Throttle value of the craft at any given point in time.
// Unit = microseconds
int current_throttle = 970;


/***** SYSTEM VARIABLES *****/

// When true, starts the series of tests.
bool test_active = false;
// Used with SD card.
const int chipSelect = 4;
// Timer used to log temperature every 10 seconds.
unsigned long temp_timer = 0;
#define MAXDO   5
#define MAXCS   6
#define MAXCLK  9
// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);


/**
 * INITIALIZES ALL REQUIRED PERIPHIALS AND DEPENDENCIES.
 */
void setup()
{
    // Creates a serial communication line between the arduino and the serial port 
    // found under 'Tools' -> 'Serial Monitor'
    Serial.begin(115200);
    // Serial communication between pins Rx & Tx of the microcontroller.
    Serial1.begin(9600);
    // Attaches pin to #.
    motor.attach(MOTOR_PIN);
    // Sends a PDM waveform that is below its cutoff value. So <0% throttle.
    motor.writeMicroseconds(700);
    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        while (1);
    }
    else{
        log_data(4);
    }
}


/**
 * Checks for the go signal from HABET and then run motor tests.
 */
void loop()
{
    // Logs temp data once every 10 seconds.
    log_data(1);
    // Checks for serial input from HABET.
    check_serial();
    // Start signal has been detected. 
    if(test_active == true)
    {
        edf_tests();
    }
}


/**
 * Runs through the various throttle tests.
 */
void edf_tests()
{
    // Ensures the test won't run more than once.
    test_active = false;

    // Acknowledges HABET's start signal.
    Serial1.write("Start");
    // Logs to the SD card that we have started the tests.
    log_data(2);

    /***** Test 1 *****/
    increment_amount = 10; // 1% throttle increment
    run_test(1040); // 5%
    delay(15000); // 15 second delay

    /***** Test 2 *****/
    increment_amount = 10; // 1% throttle increment
    run_test(1090); // 10%
    delay(15000); // 15 second delay

    /***** Test 3 *****/
    increment_amount = 50; // 5% throttle increment
    run_test(1190); // 20%
    delay(15000); // 15 second delay

    /***** Test 4 *****/
    increment_amount = 50; // 5% throttle increment
    run_test(1290); // 30%
    delay(15000); // 15 second delay

    /***** Test 5 *****/
    increment_amount = 50; // 5% throttle increment
    run_test(1390); // 40%
    delay(15000); // 15 second delay

    /***** Test 6 *****/
    increment_amount = 50; // 5% throttle increment
    run_test(1490); // 50%
    delay(15000); // 15 second delay

    // Logs to the sd card that we have stopped the tests.
    log_data(3);
    // Tells HABET that tests have concluded.
    Serial1.write("Stop");
}

/**
 * Checks for start singal via serial comms with HABET.
 */
void check_serial()
{
    if(Serial.available())
    {
        String new_input = "";
        while(Serial.available())
        {
            char t = Serial.read();
            new_input += t;
        }
        // Creates a character array of the length of the serial input.
        char toParse[new_input.length()];
        // Converts said string to character array.
        new_input.toCharArray(toParse, new_input.length());
        // Checks for the python gui starting up and attemping to
        // establish serial connection to this microcontroller.
        if(toParse[0] == '$' && test_active == false)
        {
            // Responds to the HABET that we started.
            // Serial.write("Starting.");
            // Updates connection status.
            test_active = true;
        }
    }
}

/**
  1% = 990       55% = 1540
  5% = 1040      60% = 1590
  10% = 1090     65% = 1640
  15% = 1140     70% = 1690
  20% = 1190     75% = 1740
  25% = 1240     80% = 1790
  30% = 1290     85% = 1840
  35% = 1340     90% = 1890
  40% = 1390     95% = 1940
  50% = 1490     100% = 1990
 */
void run_test(int max_throttle)
{
    // Resets the base throttle to <0%. 
    current_throttle = 970;
    // Cycles from 0% to the desired max_throttle. 
    while(max_throttle > current_throttle)
    {
        // Increments throttle amount.
        current_throttle = current_throttle + increment_amount;
        // Prints to screen for debugging. 
        //Serial.println(current_throttle);
        // Writes the new waveform to the ESC. (Increases the speed by ~1%)
        motor.writeMicroseconds(current_throttle);
        // There needs to be a certain delay between increases. 
        delay(INCREMENT_DELAY);
    }
    motor.writeMicroseconds(current_throttle);
    // Holds max throttle for 5 seconds. 
    delay(5000);
    // Shut down to below 0%.
    motor.writeMicroseconds(900);
}


/**
 * Returns the current temperature of the thermocouple.
 */
float get_external_temperature()
{
	return thermocouple.readCelsius();
}


/**
 * Logs appropriate data to sd card.
 * 1 = temperature
 * 2 = start tests
 * 3 = stop tests
 */
void log_data(int selector)
{
    // Check for temperature log. 
    // (Even though it should be 1, sometimes it 
    // comes through as 1.1 or 0.9 etc..)
    if(0.8 < selector && selector < 1.2)
    {
        // Logs temperature every 10 seconds. 
        if(millis() - temp_timer > 1000)
        {
            // Resets timer.
            temp_timer = millis();
            // Opens the text file. Create one if it doesn't exist.
            File dataFile = SD.open("edf_test.txt", FILE_WRITE);
            // Checks for valid file.
            if(dataFile)
            {
                String log = "";
                log += "Temp: ";
                log += get_external_temperature();
                log += "C";
                dataFile.println(log);
                dataFile.close();
                Serial.println(log);
            }
        }
    }
    else if(1.8 < selector && selector < 2.2)
    {
        // Opens the text file. Create one if it doesn't exist.
        File dataFile = SD.open("edf_test.txt", FILE_WRITE);
        // Checks for valid file.
        if(dataFile)
        {
            String log = "";
            log += "Starting Tests-------------------------------------------------";
            dataFile.println(log);
            dataFile.close();
            Serial.println(log);
        }
    }
    else if(2.8 < selector && selector < 3.2)
    {
        // Opens the text file. Create one if it doesn't exist.
        File dataFile = SD.open("edf_test.txt", FILE_WRITE);
        // Checks for valid file.
        if(dataFile)
        {
            String log = "";
            log += "Stoping Tests***************************************************";
            dataFile.println(log);
            dataFile.close();
            Serial.println(log);
        }
    }
    else if(3.8 < selector && selector < 4.2)
    {
        // Opens the text file. Create one if it doesn't exist.
        File dataFile = SD.open("edf_test.txt", FILE_WRITE);
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
}
