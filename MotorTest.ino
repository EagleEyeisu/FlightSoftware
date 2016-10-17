#include <Servo.h>

//This code can be used for any purpose.

Servo ESC1;

int pos = 0; //Sets position variable

void arm(){

setSpeed(0); //Sets speed variable delay(1000);

}

void setSpeed(int speed){

int angle = map(speed, 0, 100, 0, 180); //Sets servo positions to different speeds ESC1.write(angle);
  ESC1.write(angle);           /*use 180 because the arduino thinks its controlling an actual servo, so 180 degrees*/

}

void setup() {

ESC1.attach(9); //Adds ESC to certain pin. arm();

}

void loop() {

int speed; //Implements speed variable

//setSpeed(100);
//delay(5000);
setSpeed(5);
delay(8000);

for(speed = 0; speed <= 70; speed += 1) { //Cycles speed up to 70% power at (delay) second per step

setSpeed(speed); //Creates variable for speed to be used in in for loop

delay(200);

}

delay(4000); //Stays on for 4 seconds

for(speed = 70; speed > 0; speed -= 1) { // Cycles speed down to 0% power at (delay) second per step

setSpeed(speed); delay(200);

}

setSpeed(0); //Sets speed variable to zero no matter what

delay(1000); //Turns off for 1 second before lopping through again.

}
