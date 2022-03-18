// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!

#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();
 
#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 3

#define REDNOTE 60
#define GREENNOTE 61
#define BLUENOTE 62
 
#define FADESPEED 5     // make this higher to slow down
 
void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI);
}
 
 
void loop() {
  MIDI.read();

 /*
  // fade from blue to violet
  for (r = 0; r < 256; r++) { 
    analogWrite(REDPIN, r);
    delay(FADESPEED);
  } 
  // fade from violet to red
  for (b = 255; b > 0; b--) { 
    analogWrite(BLUEPIN, b);
    delay(FADESPEED);
  } 
  // fade from red to yellow
  for (g = 0; g < 256; g++) { 
    analogWrite(GREENPIN, g);
    delay(FADESPEED);
  } 
  // fade from yellow to green
  for (r = 255; r > 0; r--) { 
    analogWrite(REDPIN, r);
    delay(FADESPEED);
  } 
  // fade from green to teal
  for (b = 0; b < 256; b++) { 
    analogWrite(BLUEPIN, b);
    delay(FADESPEED);
  } 
  // fade from teal to blue
  for (g = 255; g > 0; g--) { 
    analogWrite(GREENPIN, g);
    delay(FADESPEED);
  }
  */

}

void handleNoteOn(byte channel, byte note, byte velocity) {
  byte level = velocity <= 1 ? 0 : velocity*2 + 1;
  
  if (note == REDNOTE) {
    analogWrite(REDPIN, level);
  }
  if (note == GREENNOTE) {
    analogWrite(GREENPIN, level);
  }
  if (note == BLUENOTE) {
    analogWrite(BLUEPIN, level);
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  if (note == REDNOTE) {
    analogWrite(REDPIN, 0);
  }
  if (note == GREENNOTE) {
    analogWrite(GREENPIN, 0);
  }
  if (note == BLUENOTE) {
    analogWrite(BLUEPIN, 0);
  }
}
