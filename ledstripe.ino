// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!

#include <MIDI.h>
#include <Wire.h>
#include "StripeEngine.h"

#define MIDI_CHANNEL 13
#define CC_SPEED 3
#define CC_RAINBOWSPEED 4

MIDI_CREATE_DEFAULT_INSTANCE();

StripeEngine stripeEngine;
 
void setup() {
  stripeEngine.setup();

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleControlChange);
  MIDI.begin(MIDI_CHANNEL);

  Wire.begin(); 
}

void loop() {
  MIDI.read();

  stripeEngine.loop();
}

void handleNoteOn(byte channel, byte note, byte velocity) {
  stripeEngine.setColor(note, velocity);

  Wire.beginTransmission(9);
  byte buffer[2];
  buffer[0] = note;
  buffer[1] = 127;
  Wire.write( buffer, 2);
  Wire.endTransmission(); 
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  stripeEngine.setColor(note, 0);

  Wire.beginTransmission(9);
  byte buffer[2];
  buffer[0] = note;
  buffer[1] = 0;
  Wire.write(buffer, 2);
  Wire.endTransmission(); 
}

void handleControlChange(byte channel, byte control, byte value) {
  if (control == CC_SPEED) {
    stripeEngine.setDecayTime(value);
  }
  if (control == CC_RAINBOWSPEED) {
    stripeEngine.setRainbowTime(value);
  }
}
