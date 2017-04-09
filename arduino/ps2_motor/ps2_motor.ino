#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

#include <SPI.h>
#include <PS2X_lib.h>
#include <Servo.h>

#define PS2_DAT        9      
#define PS2_CMD        11  
#define PS2_SEL        13  
#define PS2_CLK        12  

PS2X ps2x; // create PS2 Controller Class


#define SERVO_BUTTON 6  

Servo myservo;  // create servo object to control a servo
#define SERVO_UP 80
#define SERVO_DOWN 20

#define JOY_AXIS_MAX 255

#define KILLSWITCH_PIN A0
#define LED_PIN 8
#define MAX_MOTOR_SPEED 1.0


// The max speed the motor driver takes
#define MAX_MOTOR 255 
#define MIN_MOTOR -255

// Max change in motor speed per loop
#define MAX_ACCEL 25     


Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorLeft = AFMS.getMotor(4);
Adafruit_DCMotor *motorRight = AFMS.getMotor(3);

int speed_l_current;
int speed_r_current;

float f_x1;
float f_y1;

unsigned long last_trans_time;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(KILLSWITCH_PIN, INPUT_PULLUP);
  Serial.begin(115200);

  AFMS.begin();
  
  last_trans_time = millis();
  speed_l_current = 0;
  speed_r_current = 0;
  myservo.attach(10);  // attaches the servo on pin 9 to the servo object
  delay(300);
  ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);

}

void loop() {

    unsigned long msg_id;
    int i;
    float f_x1, f_y1;
    int speed_l, speed_r;
    
    digitalWrite( LED_PIN, 0);
    Serial.print("Killswitch:");
    Serial.print( digitalRead(KILLSWITCH_PIN));
    Serial.println();
    if( digitalRead( KILLSWITCH_PIN) == 1) {
         Serial.println("Killswitch detected!");
        motorLeft->setSpeed( 0 );
        motorRight->setSpeed( 0 );

        for(i=0; i<15; i++){
            Serial.println("Killswitch loop");
            digitalWrite( LED_PIN, 0);
            delay(250);
            digitalWrite( LED_PIN, 1);
            delay(250);
        }
    }
   
    ps2x.read_gamepad(false, 0); //read controller and set large motor to spin at 'vibrate' speed

    Serial.print("Stick Values:");
    Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_LX), DEC); 
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_RY), DEC); 
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_RX), DEC); 

    if(ps2x.Button(PSB_R1) ) {
        myservo.write(SERVO_DOWN);
    } else {
        myservo.write(SERVO_UP);
    }
    
   // translate joystick x/y into motor speed
   f_x1 = ps2x.Analog(PSS_RX) * 2.0 / JOY_AXIS_MAX - 1.0;
   f_y1 = ps2x.Analog(PSS_RY) * 2.0 / JOY_AXIS_MAX - 1.0;
   speed_r = (f_y1 + f_x1 * 0.5) * MAX_MOTOR;
   speed_l = (f_y1 - f_x1 * 0.5) * MAX_MOTOR;
   if( speed_l > MAX_MOTOR ) speed_l = MAX_MOTOR;
   if( speed_l < MIN_MOTOR ) speed_l = MIN_MOTOR;
   if( speed_r > MAX_MOTOR ) speed_r = MAX_MOTOR;
   if( speed_r < MIN_MOTOR ) speed_r = MIN_MOTOR;
      Serial.print(" speed_l=");
      Serial.print(speed_l);
      Serial.print(" speed_r=");
      Serial.print(speed_r);
      Serial.println();
   
   
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

//   if (millis() - last_trans_time > 150) {
//      Serial.println("No data recieved, stopping");
//      speed_l_current = 0;
//      speed_r_current = 0;
//       motorRight->run(RELEASE); 
//       motorLeft->run(RELEASE); 
//       digitalWrite( LED_PIN, 1);
//       delay(10);
//       radio.begin();
//       radio.setChannel(RADIO_CHANNEL);
//       radio.setPayloadSize(sizeof(t_data));
//       radio.setPALevel(RF24_PA_HIGH);
//       radio.openWritingPipe(addresses[1]);
//       radio.openReadingPipe(1,addresses[0]);
//       radio.startListening();

//       digitalWrite( LED_PIN, 0);
//       delay(10);
//   }
}
