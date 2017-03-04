/*******************************EAGLE EYE PROPRIETARY INFORMATION***************************************
 *                                                                                                     *
 *Purpose: Primary software for use on EAGLE EYE rotor craft. Controls and                             *
 *         maintains motor speed, deploys parachute, reads and records atmospheric                     *
 *         data.                                                                                       *
 *                                                                                                     *
 *Date:      Version:        Developer:        Description:                                            *
 *6/25/16    1.0             James Wingerter   Initial Build.                                          *
 *7/10/16    1.1             James Wingerter   Added sd memory, parachute deployment                   *
 *9/14/16    1.2             James Wingerter/Jared Danner - Added saftey count to 4                    *
 *9/19/16    1.3             Jared Danner      Changed variables to print real numbers                 *
 *12/26/16   2.0             Jared Danner      Complete Rebuild. Includes motor, altitude calculation  *            
 *                                             update, I2C Communication with LoRa, Event Logging      *
 *2/27/17    2.1             Wesley Carelton   Fixed I2C Software & Event Logging.                     *
 *******************************************************************************************************/

//LIBRARIES
/****MOTOR*******/
#include <Servo.h>

/****ALTITUDE****/
#include <SD.h> 

/****FLIGHT******/
#include <TimeLib.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>


//CONSTANT VARIABLES
/****PINS********/
#define RELAY1  28    //Parachute Digital Pin to IN1

/****MOTOR*******/
//1% = 880       30% = 1180
//5% = 930       35% = 1230
//10% = 980      40% = 1280
//15% = 1030     45% = 1330
//20% = 1080     50% = 1380
//25% = 1130     
#define MAX_SIGNAL 1180     //Change to desired max power. Percentages given right above
#define MIN_SIGNAL 700      //Dont't touch
#define MOTOR_PIN 49        //Don't touch unless switching to UNO than change to 9

#define INCREMENT_AMOUNT 10    //Don't touch
#define INCREMENT_DELAY 320    //Don't touch
#define FULL_POWER_DELAY 4000  //Changes how long you want to the motor to stay at a contant rate once you are at full power

Servo motor;                     //Creates an Servo object used to control motor.
int speed;                       //Speed variable
int speedPrevious;               //Holds the speed value that the previous cycle 
boolean spin_Up = true;          //True if spin up cycle was reached.
boolean cycle_Up = false;        //True while motor needs to throttle upward.
boolean cycle_Down = false;      //True while motor needs to throttle down.
boolean motor_Complete = false;  //True when motor cycle is complete. 
boolean motor_Start = false;     //Triggers the beginning of the motor process.
boolean break_Status = false;    //Status of break.

/****SENSORS*****/
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);   //Don't touch

/****SD CARD*****/
File EagleEyeData;                  //File used to store the flight data. Data will be written/saved to this file during flight operations
#define SD_PIN 53                   //CHANGE THIS TO MEGA OR FEATHER 

/****PARACHUTE***/
boolean chute_enable = false;       //Status of chute readiness.
boolean chute_deploy = false;       //Status of chute deployment.
int saftey_counter = 0;             //Saftey counter.
int PARACHUTE_ARM_HEIGHT = 30000;    //9144 m == 30,000 feet
int PARACHUTE_DEPLOY_HEIGHT = 20000;  //6096m == 20,000 feet

/****COMMUNICATION****/
boolean HABET_Connection = true; //Status for Connection to HABET.
boolean newData = false;         //Status of event data
int x;                           //Event Number
int MEGA_ADDRESS = 1;            //I2C Comm address for MEGA.
int LORA_ADDRESS = 2;            //I2C Comm address for LoRa.

/****MISC********/
time_t current_time;           //Time of events.

/*
 * Holds data values of Pressure, Altitude, and Temperature
 */
struct flight_data{
  float pressure;
  float altitude;
  float temperature;
};

/*
 * Method initializes the main hardware components. Only runs once, or until everything is initialized
 */
void setup() {
  Serial.begin(9600); //9600 baud, used for printing to monitor

  /****Initialize the Altimeter****/
  if(!bmp.begin()){ //BMP085 Error, check connection
    Serial.println("Pressure Sensor Offline.");
    delay(10000000);
  }
  else{
    Serial.println("Pressure Sensor Online.");
  }
  
  /****Parachute deployment Initialize****/
  //Set all the pins low so they do not toggle on Reset or Power on!
  digitalWrite(RELAY1, HIGH);  //Sends a LOW signal
  pinMode(RELAY1, OUTPUT);     //Sets RELAY1 as output pin.
  Serial.println("Parachute Online.");

  /****Initialize SD Card reader****/
  Serial.println("SD Card Online.");
  pinMode(SD_PIN, OUTPUT);
  if (!SD.begin(SD_PIN)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  /****Initialize Motor Pin****/
  motor.attach(MOTOR_PIN);    //Attaches ESC to Arduino
  Serial.println("Motor Online.");

  /****Initialize I2C Comms****/
  Wire.begin(MEGA_ADDRESS);  //Setting the address for this board.
  Serial.println("Comms Address Set.\n\n");

}
/*
 * MAIN PROGRAM CODE. 
 */
void loop(void){
  sensors_event_t event;
  bmp.getEvent(&event);
  
  if(event.pressure){
    flight_data current = getData();                                      //Updates altitude, pressure, and tempurature.
    communication(current.altitude,false,0);                              //Checks for incoming communication from LoRA.
    store_Data(current.pressure, current.temperature, current.altitude);  //Store Data to SD Card.
    parachute(current.altitude);                                          //Handles all things parachute.
    motor_Function(current.altitude);                                     //Handles motor function.
  }
  else{
    Serial.println("Sensor Error.");
  }
  if(cycle_Up || cycle_Down){
    if(spin_Up || motor_Complete){
      delay(1000);
    }
    else{
      delay(300);
    }
  }
  else{
    delay(1000);  //One second delay between recordings
  }
}

/*
 * Motor Control System
 */
void motor_Function(float Altitude){
  if(chute_deploy == false){
    if(HABET_Connection == false){
      if(motor_Complete == false){
        if(!motor_Start){
          motor_Start = true;
          cycle_Up = true;
          speedPrevious = MIN_SIGNAL;
          communication(Altitude,true,10);
        }
        else if(cycle_Up){       //CYCLING UP
          if(speed>=MAX_SIGNAL){ //Checks to see if motor has met desired throttle.
            cycle_Up == false;
            spin_Up == true;
            cycle_Down == true;
            speed = MAX_SIGNAL;
          }
          else if(speed<MAX_SIGNAL){    
            for(speed=speedPrevious;speed<=speedPrevious+70;speed+=INCREMENT_AMOUNT){
              if(speed==850){
                communication(Altitude,true,12);
                break_Status = false;
              }
              if(speed<=MAX_SIGNAL){
              motor.writeMicroseconds(speed);
              delay(100);
              }
            }
            speedPrevious = speed;
          }
       }
       else if(cycle_Down){ //CYCLING DOWN
          if(speed<=850){   //DONT TOUCH - Makes sure break is off but the motor has stopped spinning. 
            cycle_Down == false;
            motor_Complete == true;
            communication(Altitude,true,11);
          }
          else if(speed>MIN_SIGNAL){    //Checks to see if motor has stopped.
            for(speed=speedPrevious;speed>=speedPrevious-70;speed-=INCREMENT_AMOUNT){
              if(speed>=MIN_SIGNAL){
                motor.writeMicroseconds(speed);
                delay(100);
              }
            }
            speedPrevious = speed;
          }
        }
      }
      else if(!chute_deploy && chute_enable && !break_Status && Altitude <= PARACHUTE_DEPLOY_HEIGHT+300){ //<-number value is used to determine at what height above parachute deployment we need to turn the break on. WILL CHANGE
        motor.writeMicroseconds(700); //Break turns on
        break_Status = true;
        communication(Altitude,true,12);
      }
    }
  }
}

/*
 * Handles Event Logging. From LoRa and MEGA.
 *  Motor Start - 10
 *  Brake Off   - 11
 *  Brake On    - 12
 */
void communication(float Altitude, boolean Motor, int System_Event){
  if(Motor == false){
    Wire.onReceive(receiveEvent);
    if(newData){
      EagleEyeData = SD.open("EventLog.txt", FILE_WRITE);
      EagleEyeData.println();
      EagleEyeData.print(x);
      EagleEyeData.print(" <-LoRa Event Logged at ALT: ");
      EagleEyeData.print(Altitude);
      EagleEyeData.print(" at flight TIME: ");
      EagleEyeData.println(current_time = now());
      EagleEyeData.close();
      newData = false;
    }
    
  }
  else if(Motor){
    EagleEyeData = SD.open("EventLog.txt", FILE_WRITE);
    EagleEyeData.println();
    EagleEyeData.print(System_Event);
    EagleEyeData.print(" <-Mega Event Logged at ALT: ");
    EagleEyeData.print(Altitude);
    EagleEyeData.print(" at flight TIME: ");
    EagleEyeData.println(current_time = now());
    EagleEyeData.close();
  }
}

/**
 * Helper for radio
 */
void receiveEvent(){
  Serial.print("Event = ");
  x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
  newData = true;

}
 
/**
 * All parachute functions/decisions/everything.
 */
void parachute(float Altitude){
  if(!chute_enable && Altitude >= PARACHUTE_ARM_HEIGHT){    //9144 m == 30,000 feet
    saftey_counter++;
    if(saftey_counter >= 4){
      chute_enable = true;
      Serial.print("Chute enabled at ");  
      Serial.print(Altitude);
      Serial.println(" feet ");
      EagleEyeData = SD.open("FltData.txt", FILE_WRITE);
      EagleEyeData.print("Chute enabled at ");
      EagleEyeData.print(Altitude); 
      EagleEyeData.println(" feet ");
      EagleEyeData.close();
    }
    else if(Altitude <= PARACHUTE_ARM_HEIGHT){  //Resets saftey counter to 0
      saftey_counter = 0;
      Serial.println("Saftey reset to 0.");
      EagleEyeData = SD.open("FltData.txt", FILE_WRITE);
      EagleEyeData.println("Saftey reset to 0."); 
      EagleEyeData.close();  
    }
  }
  if(!chute_deploy && chute_enable && break_Status && Altitude <= PARACHUTE_DEPLOY_HEIGHT){  //6096m == 20,000 feet
    digitalWrite(RELAY1, LOW);                //This is close the circuit providing power the chute deployment system
    chute_deploy = true;
    Serial.print("Chute deployed at ");
    Serial.print(Altitude);
    Serial.println(" feet");
    delay(2000);
    digitalWrite(RELAY1, HIGH);               //Run the current for 2 seconds, then open the circuit and stop the current
    EagleEyeData = SD.open("FltData.txt", FILE_WRITE);
    EagleEyeData.print("Chute deployed at ");
    EagleEyeData.print(Altitude);
    EagleEyeData.println(" feet");
    EagleEyeData.close();
  }
}

/*
 * Updates values to current conditions.
 */
struct flight_data getData(){
  /****Get a new sensor event****/
  sensors_event_t event;
  bmp.getEvent(&event);
  
  /****Display atmospheric pressue in hPa****/
  Serial.print("Pressure:    ");
  Serial.print(event.pressure);
  Serial.println(" hPa ");
   
  /****First we get the current temperature from the BMP180****/
  float temperature;
  bmp.getTemperature(&temperature);
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C ");

  /****Calculate Altitude from Pressure****/
  float altitude = getAlt(event.pressure);
  Serial.print("Altitude:    ");
  Serial.print(altitude);
  Serial.println(" ft\n");

  /****Save current Temp, Alt, and Pressure to our data struct****/
  flight_data data;
  data.pressure = event.pressure;
  data.temperature = temperature;
  data.altitude = altitude;
  return data;
}

/*
 * Used to store Data to SD card storage
 */
void store_Data(float Pressure, float Temperature, float Altitude){
  EagleEyeData = SD.open("FltData.txt", FILE_WRITE); //USE THIS BUT EVENT LOG
  EagleEyeData.print(current_time = now());
  EagleEyeData.print(",");
  EagleEyeData.print(Altitude);

  EagleEyeData.print(",");
  EagleEyeData.print(Pressure);

  EagleEyeData.print(",");
  EagleEyeData.println(Temperature);
  EagleEyeData.close();
}

/*
 * Method used to derive Altitude from Pressure.
 * Initally hPa, than converted to mPa for calculations.
 */
float getAlt(float inPressure){
  float pressure = inPressure/10.0;
  float alt;
  float leftTop;
  float rightTop;      //All variables used to calculate altitude
  float bottom;
  float topTotal;

  if(pressure < 2.55){                           //ABOVE 25,000m
    leftTop = -47454.8;
    rightTop = pow(pressure, 0.087812) - 1.65374;
    bottom = pow(pressure, 0.087812);
    topTotal = leftTop * rightTop;
    alt = (topTotal / bottom);
  }
  else if(67.05 > pressure && pressure > 2.55){  //ABOVE 11,000m and BELOW 25,000m
    rightTop = -6369.43;
    leftTop = log(pressure) - 4.85016;
    alt =  leftTop * rightTop;
  }
  else{                                           //BELOW 11,000m (Pressure > 67.05)
    leftTop = 44397.5;
    rightTop = 18437 * pow(pressure, 0.190259);
    alt = leftTop - rightTop;
  }
  return alt*3.28; //Conversion from feet to meters
}