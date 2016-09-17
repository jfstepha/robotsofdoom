#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotorL = AFMS.getMotor(4);
Adafruit_DCMotor *myMotorR = AFMS.getMotor(3);
// You can also make another motor on port M2
//Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);           // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  // Set the speed to start, from 0 (off) to 255 (max speed)
  myMotorL->setSpeed(150);
  myMotorL->run(FORWARD);
  // turn on motor
  myMotorL->run(RELEASE);

  myMotorR->setSpeed(150);
  myMotorR->run(FORWARD);
  // turn on motor
  myMotorR->run(RELEASE);


}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println("L Forward");
  myMotorL->setSpeed(150);
  myMotorL->run(FORWARD);
  delay(1000);

  Serial.println("stop");
  myMotorL->run(RELEASE);
  myMotorR->run(RELEASE);
  delay(1000);

  Serial.println("L Reverse");
  myMotorL->setSpeed(150);
   myMotorL->run(BACKWARD);
  delay(1000);

  Serial.println("stop");
  myMotorL->run(RELEASE);
  myMotorR->run(RELEASE);
  delay(1000);

  Serial.println("R Forward");
  myMotorR->setSpeed(150);
  myMotorR->run(FORWARD);
  delay(1000);

  Serial.println("stop");
  myMotorL->run(RELEASE);
  myMotorR->run(RELEASE);
  delay(1000);

  Serial.println("R Reverse");
  myMotorR->setSpeed(150);
  myMotorR->run(BACKWARD);
  delay(1000);

  Serial.println("stop");
  myMotorL->run(RELEASE);
  myMotorR->run(RELEASE);
  delay(1000);


}
