all:
	arm-linux-gnueabihf-g++ -march=armv7-a -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -Ofast -Wall -pthread  -I/usr/local/include/RF24/.. -I.. -L/usr/local/lib my_joystick_hub.cpp -lrf24 -o my_joystick_hub
