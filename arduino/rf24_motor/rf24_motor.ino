#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

#include <SPI.h>
#include "RF24.h"

#include <Servo.h>
Servo myservo;  // create servo object to control a servo
#define SERVO_UP 180
#define SERVO_DOWN 0


#define LED_PIN 8
// MAX MOTOR SPEED, 1.0 = full speed 0.25 = quarter speed
#define MAX_MOTOR_SPEED 1.0

// The max speed the motor driver takes
#define MAX_MOTOR 255 
#define MIN_MOTOR -255

// Max change in motor speed per loop
#define MAX_ACCEL 10     
#define MAX_BUTTONS 10
#define JOY_AXIS_MAX 1024


Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorLeft = AFMS.getMotor(3);
Adafruit_DCMotor *motorRight = AFMS.getMotor(4);

RF24 radio(6,7);
byte addresses[][6] = {"1Node","2Node"};
#define RADIO_CHANNEL 2

int speed_l_current;
int speed_r_current;

struct t_data {
  uint16_t msg_id;
  int16_t x1;
  int16_t y1;
  int16_t x2;
  int16_t y2;
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
  radio.setChannel(RADIO_CHANNEL);
  radio.startListening();
  last_trans_time = millis();
  speed_l_current = 0;
  speed_r_current = 0;
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {

    unsigned long msg_id;
    int i;
    float f_x1, f_y1;
    int speed_l, speed_r;
    
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

      Serial.print(" x1=");
      Serial.print(d.x1);
      Serial.print(" y1=");
      Serial.print(d.y1);
      Serial.print(" x2=");
      Serial.print(d.x2);
      Serial.print(" y2=");
      Serial.print(d.y2);
      Serial.print(" buttons:");
      for( i=0; i<MAX_BUTTONS; i++) {
        Serial.print(d.button[i]);
      }

      
      radio.stopListening();                                        // First, stop listening so we can talk   
      radio.write( &msg_id, sizeof(unsigned long) );              // Send the final one back.      
      radio.startListening();                                       // Now, resume listening so we catch the next packets.     
      Serial.print(F(" Sent response "));
      Serial.print(msg_id);  

      
   }

   // translate joystick x/y into motor speed
   f_x1 = d.x1 * 2.0 / JOY_AXIS_MAX - 1.0;
   f_y1 = d.y1 * 2.0 / JOY_AXIS_MAX - 1.0;
   speed_l = (f_y1 + f_x1 * 0.5) * MAX_MOTOR;
   speed_r = (f_y1 - f_x1 * 0.5) * MAX_MOTOR;
   if( speed_l > MAX_MOTOR ) speed_l = MAX_MOTOR;
   if( speed_l < MIN_MOTOR ) speed_l = MIN_MOTOR;
   if( speed_r > MAX_MOTOR ) speed_r = MAX_MOTOR;
   if( speed_r < MIN_MOTOR ) speed_r = MIN_MOTOR;
      Serial.print(" speed_l=");
      Serial.print(speed_l);
      Serial.print(" speed_r=");
      Serial.print(speed_r);
      Serial.print("  msg_id=");
      Serial.println(d.msg_id);
   
   
      if ( speed_l  > speed_l_current ) {
        speed_l_current += min( MAX_ACCEL, speed_l - speed_l_current);
      }
      if ( speed_l < speed_l_current) {
        speed_l_current -= min( MAX_ACCEL, speed_l_current - speed_l);
      }
      
      if ( speed_r  > speed_r_current ) {
        speed_r_current += min( MAX_ACCEL, speed_r - speed_r_current);
      }
      if ( speed_r < speed_r_current) {
        speed_r_current -= min( MAX_ACCEL, speed_r_current - speed_r);
      }

      motorLeft->setSpeed( abs( speed_l_current ) );
      motorRight->setSpeed( abs( speed_r_current  ) );

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

   if( d.button[1] == 0 ) {
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
       radio.setChannel(RADIO_CHANNEL);
       radio.setPayloadSize(sizeof(t_data));
       radio.setPALevel(RF24_PA_HIGH);
       radio.openWritingPipe(addresses[1]);
       radio.openReadingPipe(1,addresses[0]);
       radio.startListening();

       digitalWrite( LED_PIN, 0);
       delay(150);
   }
}
