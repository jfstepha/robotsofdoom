#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

#include <SPI.h>
#include "RF24.h"

#include <Servo.h>
Servo myservo;  // create servo object to control a servo
#define SERVO_UP 120
#define SERVO_DOWN 0


#define LED_PIN 8
// MAX MOTOR SPEED, 1.0 = full speed 0.25 = quarter speed
#define MAX_MOTOR_SPEED 1.0
// Max change in motor speed per loop
#define MAX_ACCEL 10     
#define MAX_BUTTONS 10

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorLeft = AFMS.getMotor(4);
Adafruit_DCMotor *motorRight = AFMS.getMotor(3);

RF24 radio(6,7);
byte addresses[][6] = {"1Node","2Node"};

int speed_l_current;
int speed_r_current;

struct t_data {
  int16_t speed_l;
  int16_t speed_r;
  uint16_t msg_id;
  bool button[MAX_BUTTONS];
};

t_data d;
unsigned long last_trans_time;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);

  Serial.print( "Robot " );
  Serial.println( " starting");
  AFMS.begin();
  
  radio.begin();
  radio.setPayloadSize(sizeof(t_data));
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  radio.startListening();
  last_trans_time = millis();
  speed_l_current = 0;
  speed_r_current = 0;
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {

    unsigned long msg_id;
    digitalWrite( LED_PIN, 0);
    
    if( radio.available()){
      digitalWrite( LED_PIN, 1);
      last_trans_time = millis();
      Serial.print("Got something:");
                                                                    // Variable for the received timestamp
      while( radio.available() ) {
          radio.read( &d, sizeof(t_data) );             // Get the payload
      }

      msg_id = d.msg_id;
      
      radio.stopListening();                                        // First, stop listening so we can talk   
      radio.write( &msg_id, sizeof(unsigned long) );              // Send the final one back.      
      radio.startListening();                                       // Now, resume listening so we catch the next packets.     
      Serial.print(F("Sent response "));
      Serial.println(msg_id);  

      
   }
      if ( d.speed_l  > speed_l_current ) {
        speed_l_current += min( MAX_ACCEL, d.speed_l - speed_l_current);
      }
      if ( d.speed_l < speed_l_current) {
        speed_l_current -= min( MAX_ACCEL, speed_l_current - d.speed_l);
      }
      
      if ( d.speed_r  > speed_r_current ) {
        speed_r_current += min( MAX_ACCEL, d.speed_r - speed_r_current);
      }
      if ( d.speed_r < speed_r_current) {
        speed_r_current -= min( MAX_ACCEL, speed_r_current - d.speed_r);
      }

      motorLeft->setSpeed( abs( speed_l_current * MAX_MOTOR_SPEED) );
      motorRight->setSpeed( abs( speed_r_current * MAX_MOTOR_SPEED ) );

      if( speed_l_current > 0) {
        motorLeft->run(BACKWARD);
      } else if( speed_l_current < 0) {
        motorLeft->run(FORWARD);
      } else {
        motorLeft->run(RELEASE);
      }
      
      if( speed_r_current > 0) {
        motorRight->run(BACKWARD);
      } else if( speed_r_current < 0) {
        motorRight->run(FORWARD);
      } else {
        motorRight->run(RELEASE);
      }
      delay(10);
      Serial.print("  speed_l=");
      Serial.print(d.speed_l);
      Serial.print("  speed_r=");
      Serial.print(d.speed_r);
      Serial.print(" sp_l_cur=");
      Serial.print(speed_l_current);
      Serial.print(" sp_r_cur=");
      Serial.print(speed_r_current);
      Serial.print("  msg_id=");
      Serial.println(d.msg_id);

   if( d.button[0] == 1 ) {
      myservo.write(SERVO_DOWN);
   } else {
      myservo.write(SERVO_UP);
   }
   if (millis() - last_trans_time > 150) {
      Serial.println("No data recieved, stopping");
      speed_l_current = 0;
      speed_r_current = 0;
       motorRight->run(RELEASE); 
       motorLeft->run(RELEASE); 
       digitalWrite( LED_PIN, 1);
       delay(150);
       radio.begin();
       radio.setPayloadSize(sizeof(t_data));
       radio.setPALevel(RF24_PA_HIGH);
       radio.openWritingPipe(addresses[1]);
       radio.openReadingPipe(1,addresses[0]);
       radio.startListening();

       digitalWrite( LED_PIN, 0);
       delay(150);
   }

}
