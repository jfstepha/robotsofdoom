#ifndef RADIO_CHANNEL
    #define RADIO_CHANNEL 4
#endif

#define MAX_BUTTONS 11

#define SETUP_RADIO radio.begin(); \
    radio.setPALevel( RF24_PA_HIGH); \
    radio.openWritingPipe(addresses[1]); \
    radio.openReadingPipe(1, addresses[0]); \
    radio.setChannel( RADIO_CHANNEL ) ;\
    radio.startListening();


byte addresses[][6] = {"1Node","2Node"};

struct t_data {
  uint16_t msg_id;
  int16_t x1;
  int16_t y1;
  int16_t x2;
  int16_t y2;
  bool button[MAX_BUTTONS];
};

