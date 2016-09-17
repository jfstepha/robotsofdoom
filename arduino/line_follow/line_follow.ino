#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

#define SENSOR_PIN A0
#define BUMPER_PIN 2
#define THRESHOLD 660
#define LMOTOR_SPEED 110
#define RMOTOR_SPEED_L 60
#define RMOTOR_SPEED_H 130

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotorL = AFMS.getMotor(4);
Adafruit_DCMotor *myMotorR = AFMS.getMotor(3);
// You can also make another motor on port M2
//Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);

int sensor_val;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);           // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  pinMode(SENSOR_PIN, INPUT);
  pinMode(BUMPER_PIN, INPUT_PULLUP);
  

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  // Set the speed to start, from 0 (off) to 255 (max speed)
  myMotorL->setSpeed(0);
  myMotorL->run(FORWARD);
  // turn on motor
  myMotorL->run(RELEASE);

  myMotorR->setSpeed(0);
  myMotorR->run(FORWARD);
  // turn on motor
  myMotorR->run(RELEASE);
  delay(1000);
  


}

void loop() {
  // put your main code here, to run repeatedly:
  sensor_val = analogRead(SENSOR_PIN);
  
  Serial.print("sensor=");
  Serial.print(sensor_val);
  Serial.print(" bumper=");
  Serial.print(digitalRead(BUMPER_PIN));
  Serial.println();

  
  myMotorL->setSpeed(LMOTOR_SPEED);
  myMotorL->run(FORWARD);
  if (sensor_val > THRESHOLD) {
      myMotorR->setSpeed(RMOTOR_SPEED_L);    
  } else {
      myMotorR->setSpeed(RMOTOR_SPEED_H);
  }
  myMotorR->run(FORWARD);

  if(digitalRead(BUMPER_PIN) == 0 ){
    myMotorL->setSpeed(0);
    myMotorL->run(FORWARD);
    // turn on motor
    myMotorL->run(RELEASE);
  
    myMotorR->setSpeed(0);
    myMotorR->run(FORWARD);
    // turn on motor
    myMotorR->run(RELEASE);
    delay(1000);
    
  }
  

}
