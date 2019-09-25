#include <Arduino.h>
#include <Servo.h> 


/***** MOTOR VARIABLES *****/

// Specific amount to increment/decrement the PWM waveform between each pulse. 
// This ensures that each increment or decrement is suttle to not burn the motors out.
// Unit = microseconds
const byte INCREMENT_AMOUNT = 10;
// Delay time between consecutive pulses to the motors.
// Unit = milliseconds
#define INCREMENT_DELAY 320
// Physical pin connection to the Electronic Speed Controller (ESC).
// There are two 3-pin wire connectors that go to the ESC. 
// The 3-pin connector that only has 1 wire (that is white) should be connected to this pin. 
#define MOTOR_PIN 10 
// Software object that will interact with the Electronic Speed Controller. (We treat this as the EDF itself)
Servo motor;
// Throttle value of the craft at any given point in time.
// Unit = microseconds
int current_throttle = 800;



/***** SYSTEM VARIABLES *****/

// When true, starts the series of tests.
bool test_active = false;



/**
 * INITIALIZES ALL REQUIRED PERIPHIALS AND DEPENDENCIES.
 */
void setup()
{
    // Creates a serial communication line between the arduino and the serial port 
    // found under 'Tools' -> 'Serial Monitor'
    Serial.begin(115200);
    // Attaches pin to 
    motor.attach(MOTOR_PIN);
    // Sends a PDM waveform that is below its cutoff value. So <0% throttle.
    motor.writeMicroseconds(700);
}


/**
 * MAIN PROGRAM CODE.
 */
void loop()
{
    while(900 > current_throttle)
     {
        // Increments throttle amount.
        current_throttle = current_throttle + INCREMENT_AMOUNT;
        // Prints to screen for debugging. 
        Serial.println(current_throttle);
        // Writes the new waveform to the ESC. (Increases the speed by ~1%)
        motor.writeMicroseconds(current_throttle);
        // There needs to be a certain delay between increases. 
        delay(INCREMENT_DELAY);
    }
    motor.writeMicroseconds(current_throttle);
    // Holds max throttle for 5 seconds. 
    delay(5000);
    // Shut down to below 0%.
    motor.writeMicroseconds(700);
    //Delays long enough for me to unplug it. 
    delay(1000000000);
}


// /**
//  * Increments the throttle up to the max given for each individual test. 
//  */
// void run_test(int throttle_input)
// {
//     // Each test starts at 0 throttle or a 900 microsecond pwm waveform. 
//     current_throttle = 900;
//     // Takes in a value 0 - 100 and converts that to the wanted pwm waveform. 
//     int max_throttle = convert_throttle(throttle_input);
//     Serial.print("Throttle Target: "); Serial.println(max_throttle);
//     // Cycles up to the given max throttle percentage. 
//     while(max_throttle > current_throttle)
//     {
//         // Increments throttle amount.
//         current_throttle = current_throttle + INCREMENT_AMOUNT;
//         // Prints to screen for debugging. 
//         Serial.println(current_throttle);
//         // Writes the new waveform to the ESC. (Increases the speed by ~1%)
//         motor.writeMicroseconds(current_throttle);
//         // There needs to be a certain delay between increases. 
//         delay(INCREMENT_DELAY);
//     }
//     motor.writeMicroseconds(current_throttle);
// }


// /**
//  * Converts the inputted max throttle value to the appropriate 
//  * microsecond value. 
//  */
// int convert_throttle(int input_throttle)
// {
//     int throttle = input_throttle * 10;
//     throttle = throttle + 980;
//     return throttle;
// }
