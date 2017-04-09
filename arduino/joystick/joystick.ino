#define P_X1 A0
#define P_Y1 A1
#define P_X2 A2
#define P_Y2 A3
#define P_B1 2
#define P_B2 3
#define P_B3 4
#define P_B4 5
#define P_B5 8
#define P_B6 9
#define P_B7 A4
#define P_B8 A5
#define P_B9 A6
#define P_B10 A7
#define MAX_BUTTONS 11 // array starts at 0, but we're only using 1-10
#define LED_PIN 10


#include <SPI.h>
#include "RF24.h"

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(6,7);
#define RADIO_CHANNEL 5
byte addresses[][6] = {"1abcd","2abcd"};

struct t_data {
  uint16_t msg_id;
  int16_t x1;
  int16_t y1;
  int16_t x2;
  int16_t y2;
  bool button[MAX_BUTTONS];
};

t_data d;
uint16_t msg_id;
unsigned long response;

void setup() {
  // put your setup code here, to run once:
  int i;
  
  Serial.begin(115200);
  Serial.print("Joystick starting on channel ");
  Serial.print( RADIO_CHANNEL );
  Serial.println(".");

  pinMode(LED_PIN, OUTPUT);

  pinMode( P_X1, INPUT );
  pinMode( P_Y1, INPUT );
  pinMode( P_B1, INPUT_PULLUP );
  pinMode( P_X2, INPUT );
  pinMode( P_Y2, INPUT );
  pinMode( P_B2, INPUT_PULLUP );
  pinMode( P_B3, INPUT_PULLUP );
  pinMode( P_B4, INPUT_PULLUP );
  pinMode( P_B5, INPUT_PULLUP );
  pinMode( P_B6, INPUT_PULLUP );
  pinMode( P_B7, INPUT_PULLUP );
  pinMode( P_B8, INPUT_PULLUP );
  pinMode( P_B9, INPUT_PULLUP );
  pinMode( P_B10, INPUT_PULLUP );

  radio.begin();
//  radio.setPayloadSize(sizeof(t_data));
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);
  radio.setChannel(RADIO_CHANNEL);
  radio.stopListening();

  d.x1=0;
  d.y1=0;
  d.x2=0;
  d.y2=0;
  for( i=0; i<MAX_BUTTONS; i++) {
    d.button[i] = 0;
  }
  msg_id = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  int x1,y1, x2, y2;
  bool b1,b2,b3,b4,b5,b6,b7,b8,b9,b10;
  int response;
  digitalWrite(LED_PIN, HIGH);

  x1 = analogRead( P_X1 );
  y1 = analogRead( P_Y1 );
  b1 = digitalRead( P_B1 );
  
  x2 = analogRead( P_X2 );
  y2 = analogRead( P_Y2 );
  b2 = digitalRead( P_B2 );

  b3 = digitalRead( P_B3 );
  b4 = digitalRead( P_B4 );
  b5 = digitalRead( P_B5 );
  b6 = digitalRead( P_B6 );
  b7 = digitalRead( P_B7 );
  b8 = digitalRead( P_B8 );

  // Pins analog 6 and analog 7 do not have internal pullups!
  // they need explicit resistors added in the circuit.
  //b9 = digitalRead( P_B9 );
  //b10 = digitalRead( P_B10 );
  b9 = analogRead( P_B9 ) > 250 ? 1 : 0;
  b10 = analogRead( P_B10 ) > 250  ? 1 : 0;

  Serial.print(" X1:");
  Serial.print(x1);
  Serial.print(" Y1:");
  Serial.print(y1);
  Serial.print(" B1:");
  Serial.print(b1);
  Serial.print(" X2:");
  Serial.print(x2);
  Serial.print(" Y2:");
  Serial.print(y2);
  Serial.print(" B2:");
  Serial.print(b2);

  Serial.print(" Btns:");
  Serial.print(b3);
  Serial.print(b4);
  Serial.print(b5);
  Serial.print(b6);
  Serial.print(b7);
  Serial.print(b8);
  Serial.print(b9);
  Serial.print(b10);
  Serial.print(" ");
  Serial.print(digitalRead(P_B9));
  Serial.print(digitalRead(P_B10));

  //Serial.println("");

  d.x1 = x1;
  d.y1 = y1;
  d.x2 = x2;
  d.y2 = y2;
  d.button[1] = b1;
  d.button[2] = b2;
  d.button[3] = b3;
  d.button[4] = b4;
  d.button[5] = b5;
  d.button[6] = b6;
  d.button[7] = b7;
  d.button[8] = b8;
  d.button[9] = b9;
  d.button[10] = b10;
  d.msg_id = msg_id++;

  digitalWrite(LED_PIN, LOW);
  
  unsigned long start_time = micros();   

//  radio.stopListening();
  
  if (!radio.write( &d, sizeof(t_data) )){
    Serial.print(F(" SEND FAILED "));
  } else {
    Serial.print(F(" sndOK"));
  }
  Serial.println("Sent d");
//  radio.startListening();

 
//     unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
//    boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
    
//    while ( ! radio.available() ){                             // While nothing is received
//      if (micros() - started_waiting_at > 20000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
//          timeout = true;
//          break;
//      }      
//    }
//
//    if ( timeout ){                                             // Describe the results
//        Serial.println(F("Failed, response timed out."));
//    }else{
//        unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
//        radio.read( &got_time, sizeof(unsigned long) );
//        unsigned long end_time = micros();
//        Serial.print(F(", resp:"));
//        Serial.print(got_time);
//        Serial.print(F(", rtd:"));
//        Serial.print(end_time-start_time);
//        Serial.println("");
//    }
//    delay(1);
}
