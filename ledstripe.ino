// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!

#include <MIDI.h>
#include <Wire.h>
#include "StripeEngine.h"

#define MIDI_CHANNEL 10
#define CC_SPEED 3
#define CC_RAINBOWSPEED 4
#define CC_EXPLODE_LEVEL 5

MIDI_CREATE_DEFAULT_INSTANCE();

StripeEngine stripeEngine;
 
void setup() {
  stripeEngine.setup(false);

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

void transmit(byte canal, byte dataA, byte dataB) {
  Wire.beginTransmission(9);
  byte buffer[3] = {canal, dataA, dataB};
  Wire.write(buffer, 3);
  Wire.endTransmission(); 
}

void handleNoteOn(byte channel, byte note, byte velocity) {
  stripeEngine.setNote(note, velocity);
  transmit(0, note, velocity);
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  stripeEngine.setNote(note, 0);
  transmit(0, note, 0);
}

void handleControlChange(byte channel, byte control, byte value) {
  if (control == CC_SPEED) {
    stripeEngine.setDecayTime(value);
    transmit(1, control, value);
  }
  if (control == CC_RAINBOWSPEED) {
    stripeEngine.setRainbowTime(value);
    transmit(1, control, value);
  }
  if (control == CC_EXPLODE_LEVEL) {
    stripeEngine.setExplodeLevel(value);
    transmit(1, control, value);
  }
}
