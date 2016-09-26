
/*                        EAGLE EYE PROPRIETARY INFORMATION                                           *
 *                                                                                                     *
  Purpose: Primary software for use on EAGLE EYE rotor craft. Controls and
           maintains motor speed, deploys parachute, reads and records atmospheric
           data.
 *                                                                                                     *
  Date:      Version:        Developer:        Description:
  6/25/16    1.0             James Wingerter   Initial Build.
  7/10/16    1.1             James Wingerter   added sd memory, motor control, parachute deployment
 *9/14/16    1.2             James Wingerter/Jarod Danner - added saftey count to 4                                                                                                     *
 *9/26/16    1.3             James Wingerter   Data recording fix                                      *
 *                                                                                                     *
*/
#include <Time.h>
#include <TimeLib.h>
#include <Servo.h>
#include <Wire.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

boolean chute_enable = false;  /*initialize enabled as false until it has been confirmed to be enabled*/
boolean chute_deploy = false;  /*initialize deployment as false (because it hasn't happened at the beginning of the flight)*/
// boolean motor_start = false;   /*intialize motor start as false because it doesn't turn on until ~100,000 feet. */
time_t start;
time_t current_time;
File EagleEyeData;      /* File used to store the flight data. Data will be written/saved to this file during flight operations  */
int secondcount = 1;      /* use this count to keep track of time for writing to the file */
int saftey_counter = 0;   /* counter that must reach 4  before deployment */


int value = 0;        /* set values you need to zero*/
//************
// Servo ESC;   //create a servo object for the ESC control. It is easier to use native servo library
//
//************
//
// int speed;  //varaible that contains the speed of the motor
// int pos = 0;    // variable to store the servo position for ESC
//
//  define names for the Digital pin 6 On the Arduino
//  These data pins link to the Relay board pins "IN1"
#define RELAY1  6

struct flight_data {
  int pressure;
  int temperature;
  int Alt;
};

/*****************************/
//void arm() {
//
//  setSpeed(0); //Sets speed variable delay(1000);
//
//}
/*****************************/
//void setSpeed(int speed) {
//
//  int angle = map(speed, 0, 100, 0, 180); //Sets servo positions to different speeds.
//  ESC.write(angle);           /*use 180 because the arduino thinks its controlling an actual servo, so 180 degrees*/
//}

/************************The setup() method initializes the main hardwars components. Only runs once, or until everything is initialized********************/
void setup() {
//  ESC.attach(9);    /* attatch the ESC to digital pin 9 on the arduino */
//  arm();
  /*Serial.begin(9600);    // start serial at 9600 baud*/

  //myservo.attach(9);  // attaches the servo on pin 9 to the servo object



  Serial.begin(9600); //9600 baud
  Serial.println("Pressure Sensor Test"); Serial.println("");

  /***** Initialize the Altimeter ****/
  boolean detected = false;
  while (not detected) {

    if (!bmp.begin())
    {
      /* There was a problem detecting the BMP085 ... check your connections */
      Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
      delay(250);
    }
    else {
      detected = true;
    }
  }
  /*****Parachute deployment initialize******/
  //Set all the pins low so they do not toggle on Reset or Power on!
  digitalWrite(6, HIGH); /*send a LOW signal to pin 6*/

  // Initialise the Arduino data pins for OUTPUT
  pinMode(RELAY1, OUTPUT);

  /************* initialize SD Card reader********/
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(10, OUTPUT);
//
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}

/********* method used to store Data to SD card storage***************/

void storeData(int Pressure, int Temperature, int Altitude) {
  /*****Write Recorded data to SD Card File every second******/
  /*TODO: Replace secondcount with now() */
  current_time = now();
  EagleEyeData = SD.open("FltData.txt", FILE_WRITE);
  /*EagleEyeData.print(secondcount);*/
  EagleEyeData.print(current_time);
  EagleEyeData.print(", ");
  EagleEyeData.print(Altitude);
//
  EagleEyeData.print(", ");
  EagleEyeData.print(Pressure);
//
  EagleEyeData.print(", ");
  EagleEyeData.print(Temperature);
  EagleEyeData.println("");

  EagleEyeData.close();   /* flush() physically saves the data to the card, so that the data will be retrievble even if the Arduino gets damaged.  */
}





/****** used to calculate flight data and return values in a struct, which will be called in the main loop ******/

struct flight_data getData() {
  /* Get a new sensor event */
  sensors_event_t event;
  bmp.getEvent(&event);
  /* Display atmospheric pressue in hPa */
  Serial.print("Pressure:    ");
  Serial.print(event.pressure);
  Serial.println(" hPa");

  /* Calculating altitude with reasonable accuracy requires pressure   *
    sea level pressure for your position at the moment the data is
    converted, as well as the ambient temperature in degress
    celcius.  If you don't have these values, a 'generic' value of
    1013.25 hPa can be used (defined as SENSORS_PRESSURE_SEALEVELHPA
    in sensors.h), but this isn't ideal and will give variable
    results from one day to the next.
  *                                                                    *
    You can usually find the current SLP value by looking at weather
    websites or from environmental information centers near any major
    airport.
  *                                                                    *
    For example, for Paris, France you can check the current mean
    pressure and sea level at: http://bit.ly/16Au8ol                   */

  /* First we get the current temperature from the BMP180 */
  float temperature;
  bmp.getTemperature(&temperature);
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  /* Then convert the atmospheric pressure, SLP (Sea level pressue) and temp to altitude    */
  /* Update this next line with the current SLP for better results                  */
  float seaLevelPressure = 1014.8;  /***CHANGE TO DES MOINES AIRPORT VALUES*** OR Wherever the launch takes place*/
  Serial.print("Altitude:    ");
  Serial.print(bmp.pressureToAltitude(seaLevelPressure, event.pressure, temperature));
  float pressure = event.pressure;
  float Alt = bmp.pressureToAltitude(seaLevelPressure, event.pressure, temperature);  /*variable that contains the current altitude measurement*/
  flight_data data;
  data.pressure = event.pressure;
  data.temperature = temperature;
  data.Alt = Alt;
  return data;
}

/***********the loop() method is the main code, running constantly as Eagle Eye is in operation ******************/
/*****************************************************************************************************************/
void loop(void)
{

  /* Get a new sensor event */
  sensors_event_t event;
  bmp.getEvent(&event);

  /* Display the results (barometric pressure is measure in hPa) */
  if (event.pressure)
  {
    struct flight_data current = getData();

    /*Record data to the SD card*/
    storeData(current.pressure, current.temperature, current.Alt);

    /*****Parachute Deployment (using servo)***********/
    
    EagleEyeData = SD.open("FltData.txt", FILE_WRITE);
    if (not chute_enable and current.Alt >= 279) {  /*9144 m == 30,000 feet*/
      saftey_counter = saftey_counter + 1;               /* Increment the saftey_counter by 1 until */
      if (saftey_counter >= 4){
        chute_enable = true;
        Serial.print("chute enabled at ");
        Serial.print(current.Alt);
        Serial.print(" meters \n");
        EagleEyeData.print("chute enabled at ");
        EagleEyeData.print(current.Alt);
        EagleEyeData.print(" meters \n");
      }
      else if(current.Alt <= 279){
        saftey_counter = 0;  /* reset to zero */
        //Serial.print("the saftey counter has been reset to zero");      
      }
    }

    if (not chute_deploy and chute_enable and current.Alt <= 277) { /*6096m == 20,000 feet*/
      //myservo.write(180); //open the servo to 180 degrees
      //digitalWrite(RELAY1, LOW); /*This is close the circuit providing power the chute deployment system*/
      chute_deploy = true;
      Serial.print("chute deployed at ");
      Serial.print(current.Alt);
      //delay(2000);
      digitalWrite(RELAY1, HIGH); /*run the current for 2 seconds? then open the circuit and stop the current*/
      EagleEyeData.print("chute deployed at ");
      EagleEyeData.print(current.Alt);
      EagleEyeData.print(" meters");
      EagleEyeData.println("");
    }
    EagleEyeData.close();   /* flush() physically saves the data to the card, so that the data will be retrievble even if the Arduino gets damaged.  */
    
    
  
    /********** Motor functions ************/
//    if (not motor_start and current.Alt >= 30510.48) { //turn on the motor at 100,100 feet (allow room for free fall until sustained hover)
//      //ESC.writeMicroseconds(value);
//      motor_start = true;
//      time_t start = now();
//
//      int speed; //Implements speed variable
//      for (speed = 0; speed <= 70; speed += 5) { /*Cycles speed up to 70% power for 1 second duration increment intervals*/
//
//        /*the program will still record data during this spin up*/
//        struct flight_data spinup = getData();
//        storeData(spinup.pressure, spinup.temperature, spinup.Alt);
//        setSpeed(speed); //Creates variable for speed to be used in for loop
//        delay(1000);   /*delay for 1 second before incrementing speed up 5%. (This can change if needed)*/
//       /*takes 14 seconds to reach desired 70%*/
//      }
//
//    }
//
//
//    if (motor_start) {
//     time_t current = now();
//      if (second(current) - second(start) > 120) {
//        motor_start =  false;
//
//        for (speed = 70; speed > 0; speed -= 10) { // Cycles speed down to 0% power in 1 second increments. (7 seconds total)
//          struct flight_data spindown = getData();
//          storeData(spindown.pressure, spindown.temperature, spindown.Alt); /*Write SD Storage*/
//          setSpeed(speed);
//          delay(1000);
//
//
//        }
//        /*value = 0;  after 120 seconds, turn off motor*/
//        setSpeed(0); //Sets speed variable no matter what after this event
//        /**************************/
//      }
//
//    }

  }
  else {
    Serial.println("Sensor error");
  }
  secondcount = secondcount + 1; /*increment the time elapsed by 1 second*/
  delay(1000);  /*delay 1 second before reading the next value*/

}
