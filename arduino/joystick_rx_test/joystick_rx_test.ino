#include <Wire.h>

#include <SPI.h>
#include "RF24.h"

#define RADIO_CHANNEL 4
#define LED_PIN 8
#define MAX_BUTTONS 11 // array starts at 0, but we're only using 1-10
#define WPR_MAX 100
#define PRINT_INTERVAL 1000

RF24 radio(6,7);
byte addresses[][6] = {"1abcd","2abcd"};

struct t_data {
  uint16_t msg_id;
  int16_t x1;
  int16_t y1;
  int16_t x2;
  int16_t y2;
  bool button[MAX_BUTTONS];
};

uint16_t prev_msg_id = 0;

t_data d;
unsigned long last_trans_time;

// weighted ping ratio?  The number of msg ID's we missed
int wpr = 0;
float wpr_mean = 0;

// ping loop ratio - how many times through the loop before we get a ping

float plr = 0.0;
float plr_mean = 0;

// loops since recieved - how many loops its been since we recieved data
int lsr = 0;
int missed_msgs = 0;

int loops_since_print = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);

//  Serial.print( "Starting" );
//  Serial.print( " on channel ");
//  Serial.print( RADIO_CHANNEL );
//  Serial.println( ".");
  
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  radio.setChannel(RADIO_CHANNEL);
  radio.startListening();
  last_trans_time = millis();
}

void loop() {

    unsigned long msg_id;
    int i;
    
    digitalWrite( LED_PIN, 0);
    
    if( radio.available()){
        digitalWrite( LED_PIN, 1);
        while( radio.available() ) {
            radio.read( &d, sizeof(t_data) );             // Get the payload
        }

        msg_id = d.msg_id;

        if (prev_msg_id == 0) {
          prev_msg_id = msg_id;
        }
        if (msg_id - prev_msg_id > 1) {
            // if we missed a message, decrease WPR
            missed_msgs += (msg_id - prev_msg_id);
            if (wpr > 0) {
                wpr = wpr - 1;
            }
        } else {
            if (wpr < WPR_MAX) {
                wpr = wpr + 1;
            } 
        }
        lsr = 0;
        prev_msg_id = msg_id;
//        Serial.print("msg_id=");
//        Serial.print(msg_id);
//        Serial.print(" prev_msg_id=");
//        Serial.print(prev_msg_id);
//        Serial.print(" missed_msgs=");
//        Serial.print(missed_msgs);
//        Serial.println();
    }  // end if message recieved
    
    if (lsr == 0) {
        // if we recieved something in this loop
        plr = plr * 0.99 + 0.01;
    } else {
        plr = plr * 0.99;
    }
    lsr = lsr + 1;

    if (missed_msgs > 100 || missed_msgs < 0) {
      missed_msgs = 0;
    }
      
    if (loops_since_print < PRINT_INTERVAL) {
        plr_mean = plr_mean + plr;     
        wpr_mean = wpr_mean + wpr;
    } else {
          Serial.print( missed_msgs );
          Serial.print(" ");          
          Serial.print( wpr );
          Serial.print(" ");          
          Serial.print( (wpr_mean / PRINT_INTERVAL) );
          Serial.print(" ");
          Serial.print( (plr_mean / PRINT_INTERVAL) * 1000);
          Serial.println();
          plr_mean = 0;
          wpr_mean = 0;
          loops_since_print=0;
    } 
    loops_since_print += 1;

    digitalWrite( LED_PIN, 0);
}
