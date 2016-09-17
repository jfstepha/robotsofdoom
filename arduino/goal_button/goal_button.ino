#define SERVO_PIN 3
#define BTN_PIN 2
#define FLAG_LOOPS 2
#define LED_PIN_1 4
#define LED_PIN_2 5
#define LED_PIN_3 6
#define LED_PIN_4 7

#include <Servo.h>

Servo myservo;  // create servo object to control a servo

bool btn;
bool won;
int flag_loop;

const int buzzerPin = 9;
const int songLength = 18;
char notes[] = "ced cef ceg caa ab"; // a space represents a rest
int beats[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
int tempo = 150;


void setup() {
  // put your setup code here, to run once:
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  Serial.begin(115200);
  myservo.attach(SERVO_PIN);
  myservo.write( 0 );
  won = 0;
  flag_loop=0;

  pinMode(buzzerPin, OUTPUT); 
}


void loop() {
  // put your main code here, to run repeatedly:
  btn = digitalRead( BTN_PIN );
  if( btn==0 ) won = 1;
  
  Serial.print("button:");
  Serial.print(btn);
  Serial.print(" won:");
  Serial.print(won);
  Serial.println();

  if( won ) {
    int i, duration;
    myservo.attach(SERVO_PIN);
  
    for (i = 0; i < songLength; i++) // step through the song arrays
    {
      if ( i % 4 == 0) {
        digitalWrite( LED_PIN_4, 0);
        digitalWrite( LED_PIN_1, 1);
      }
      if ( i % 4 == 1) {
        digitalWrite( LED_PIN_1, 0);
        digitalWrite( LED_PIN_2, 1);
      }
      if ( i % 4 == 2) {
        digitalWrite( LED_PIN_2, 0);
        digitalWrite( LED_PIN_3, 1);
      }
      if ( i % 4 == 3) {
        digitalWrite( LED_PIN_3, 0);
        digitalWrite( LED_PIN_4, 1);
      }
      if ( i % 2 == 0) {
        myservo.write( 120 );
        
      } else {
        myservo.write( 60 );
        
      }
      duration = beats[i] * tempo;  // length of note/rest in ms
      
      if (notes[i] == ' ')          // is this a rest? 
      {
        delay(duration);            // then pause for a moment
      }
      else                          // otherwise, play the note
      {
        tone(buzzerPin, frequency(notes[i]), duration);
        delay(duration);            // wait for tone to finish
      }
      delay(tempo/10);              // brief pause between notes
    }

    flag_loop += 1;
    if( flag_loop > FLAG_LOOPS ) {
      won=0;
      flag_loop = 0; 
        digitalWrite( LED_PIN_1, 0);
        digitalWrite( LED_PIN_2, 0);
        digitalWrite( LED_PIN_3, 0);
        digitalWrite( LED_PIN_4, 0);
    }
  } else {
    delay(100);
    myservo.write(0);
    delay(100);
    myservo.detach();
  }
}

int frequency(char note) 
{
  // This function takes a note character (a-g), and returns the
  // corresponding frequency in Hz for the tone() function.
  
  int i;
  const int numNotes = 8;  // number of notes we're storing
  
  // The following arrays hold the note characters and their
  // corresponding frequencies. The last "C" note is uppercase
  // to separate it from the first lowercase "c". If you want to
  // add more notes, you'll need to use unique characters.

  // For the "char" (character) type, we put single characters
  // in single quotes.

  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494, 523};
  
  // Now we'll search through the letters in the array, and if
  // we find it, we'll return the frequency for that note.
  
  for (i = 0; i < numNotes; i++)  // Step through the notes
  {
    if (names[i] == note)         // Is this the one?
    {
      return(frequencies[i]);     // Yes! Return the frequency
    }
  }
  return(0);  // We looked through everything and didn't find it,
              // but we still need to return a value, so return 0.
}

