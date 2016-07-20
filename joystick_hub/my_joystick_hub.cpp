/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 03/17/2013 : Charles-Henri Hallard (http://hallard.me)
              Modified to use with Arduipi board http://hallard.me/arduipi
						  Changed to use modified bcm2835 and RF24 library
TMRh20 2014 - Updated to work with optimized RF24 Arduino library

 */

/**
 * Example RF Radio Ping Pair
 *
 * This is an example of how to use the RF24 class on RPi, communicating to an Arduino running
 * the GettingStarted sketch.
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>
#include <stdio.h>
#define JOY_AXIS_MAX 32767
#define MOTOR_MAX 255
#define MAX_BUTTONS 10 
using namespace std;

// data structure for sending data 
struct t_send {
  int16_t speed_l;
  int16_t speed_r;
  uint16_t msg_id;
  bool button[MAX_BUTTONS];
};

t_send s;

//
// Hardware configuration

// Configure the appropriate pins for your connections

/****************** Raspberry Pi ***********************/


// RPi generic:
RF24 radio(22,0);

/********** User Config *********/
// Assign a unique identifier for this node, 0 or 1
bool radioNumber = 0;

/********************************/

// Radio pipe addresses for the 2 nodes to communicate.
const uint8_t pipes[][6] = {"1Node","2Node"};


int main(int argc, char** argv){

  int msg_id=0;
  int i;

  cout << "Joystick hub starting\n";

/************* joystick stuff  ************/
	int joy_fd, *axis=NULL, num_of_axis=0, num_of_buttons=0, x;
	char *button=NULL, name_of_joystick[80];
	struct js_event js;
  	if( ( joy_fd = open ("/dev/input/js0", O_RDONLY) ) == -1 ) {
     		printf("Couldn't open joystick\n");
     		return -1;
  	}

	ioctl( joy_fd, JSIOCGAXES, &num_of_axis );
	ioctl( joy_fd, JSIOCGBUTTONS, &num_of_buttons );
	ioctl( joy_fd, JSIOCGNAME(80), &name_of_joystick );

	axis = (int *) calloc( num_of_axis, sizeof( int ) );
	button = (char *) calloc( num_of_buttons, sizeof( char ) );

	printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n"
		, name_of_joystick
		, num_of_axis
		, num_of_buttons );

	fcntl( joy_fd, F_SETFL, O_NONBLOCK );	/* use non-blocking mode */



  // Setup and configure rf radio
  radio.begin();
  radio.setPayloadSize(sizeof(t_send));
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setPALevel(RF24_PA_HIGH);
  // Dump the configuration of the rf unit for debugging
  radio.printDetails();


/***********************************/

	radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
	
	radio.startListening();
	unsigned long last_send_time = 0;
	
	// forever loop
	while (1) {
			// First, stop listening so we can talk.
			radio.stopListening();
			bool do_send = 0;

			read(joy_fd, &js, sizeof(struct js_event));
			switch( js.type & ~JS_EVENT_INIT)
			{
				case JS_EVENT_AXIS:
					axis[ js.number ] = js.value;
					break;
				case JS_EVENT_BUTTON:
					button[ js.number ] = js.value;
					break;
			}
			/* Translate x/y to motors */
			float f_x = (axis[0] * 1.0 /JOY_AXIS_MAX);
			float f_y = (axis[1] * 1.0 /JOY_AXIS_MAX);
			float motor_l = f_y + f_x * 0.5;
			float motor_r = f_y - f_x * 0.5;
			if (motor_l > 1) motor_l = 1;
			if (motor_r > 1) motor_r = 1;
			if (motor_l < -1) motor_l = -1;
			if (motor_r < -1) motor_r = -1;

			unsigned long time = millis();
			if (time - last_send_time > 100) {
			/* print the results */
			printf( "X: %6d  Y: %6d  ", axis[0], axis[1] );
		
				if( num_of_axis > 2 )
					printf("Z: %6d  ", axis[2] );
			
				if( num_of_axis > 3 )
					printf("R: %6d  ", axis[3] );
			
				for( x=0 ; x<num_of_buttons ; ++x )
					printf("B%d: %d  ", x, button[x] );
				printf("motors: %f/%f", motor_l, motor_r);

				printf("  \r");
				fflush(stdout);
				do_send = 1;
			}
			if(do_send) {
				last_send_time = millis();

				s.speed_l = motor_l * MOTOR_MAX;
				s.speed_r = motor_r * MOTOR_MAX;
				s.msg_id = ++msg_id;
				for( i=0; i<MAX_BUTTONS && i < num_of_buttons; i++){
					if (i<num_of_buttons) {
						s.button[i] = button[i];
					} else {
						s.button[i] = 0;
					}
				}


				printf("Now sending message # %d...\n",msg_id);

				bool ok = radio.write( &s, sizeof(t_send) );

				if (!ok){
					printf("failed.\n");
				}
				// Now, continue listening
				radio.startListening();

				// Wait here until we get a response, or timeout
				unsigned long started_waiting_at = millis();
				bool timeout = false;
				while ( ! radio.available() && ! timeout ) {
					if (millis() - started_waiting_at > 20 )
						timeout = true;
				}


				// Describe the results
				if ( timeout ) {
					printf("Failed, response timed out.\n");
				}
				else {
					// Grab the response, compare, and send to debugging spew
					unsigned long got_msgid;
					radio.read( &got_msgid, sizeof(unsigned long) );

					// Spew it
					printf("Got response %lu\n",got_msgid);
				}
			} // if do_send
	} // forever loop

  return 0;
}

