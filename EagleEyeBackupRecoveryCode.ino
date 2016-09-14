/* EAGLE EYE
 *  Backup Recovery code
 *  
 *  Based off of:
 *    EagleEyeRecoveryTestingCode_v3.ino  By James Wingerter  Version: 1.1
 *  
 *   Date:        Version:    Developer:          Description:
 *   9/12/2016    1.0         Wesley Carleton     Initial Build.
 *   
  */
// #include <Time.h>
// #include <TimeLib.h>
// #include <Servo.h>
#include <Wire.h>
// #include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#define RELAY1  6
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

boolean chute_enable = false;  /*initialize enabled as false until it has been confirmed to be enabled*/
boolean chute_deploy = false;  /*initialize deployment as false (because it hasn't happened at the beginning of the flight)*/

struct flight_data {
  int pressure;
  int temperature;
  int Alt;
};

/************************The setup() method initializes the main hardwars components. Only runs once, or until everything is initialized********************/
void setup() {
 
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
}

/****** used to calculate flight data and return values in a struct, which will be called in the main loop ******/

struct flight_data getData() {
  /* Get a new sensor event */
  sensors_event_t event;
  bmp.getEvent(&event);
  /* Display atmospheric pressue in hPa */

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

  /* Then convert the atmospheric pressure, SLP (Sea level pressue) and temp to altitude    */
  /* Update this next line with the current SLP for better results                  */
  
  float seaLevelPressure = 1024.3;  /***CHANGE TO DES MOINES AIRPORT VALUES*** OR Wherever the launch takes place*/
  
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

if (event.pressure)
  {
    struct flight_data current = getData();

    /*****Parachute Deployment (using servo)***********/
    
    if (not chute_enable and current.Alt >= 295) {  /*9144 m == 30,000 feet*/
      chute_enable = true;
   
    }

    if (not chute_deploy and chute_enable and current.Alt <= 293) { /*6096m == 20,000 feet*/
      digitalWrite(RELAY1, LOW); /*This is close the circuit providing power the chute deployment system*/
      chute_deploy = true;
      delay(2000);
      digitalWrite(RELAY1, HIGH); /*run the current for 2 seconds? then open the circuit and stop the current*/
      
    }
   else {
    Serial.println("Sensor error");
   }
  }
}
