// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!

#include <MIDI.h>

#define MIDI_CHANNEL 13
#define CC 3

MIDI_CREATE_DEFAULT_INSTANCE();
 
#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 3

#define REDNOTE 60
#define GREENNOTE 61
#define BLUENOTE 62

unsigned long decayTime = 1;

struct ColorStatus {
  int level = 0;
  byte fullLevel = 0;
  bool isOn = false;
  unsigned long timeStamp = 0;
  byte pin;
};

enum Color {
  red = 0,
  green,
  blue,

  count
};

ColorStatus colorStatus[count];
unsigned long timePassed;
 
void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  colorStatus[red].pin = REDPIN;
  colorStatus[green].pin = GREENPIN;
  colorStatus[blue].pin = BLUEPIN;

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleControlChange);
  MIDI.begin(MIDI_CHANNEL);

  for (byte c = 0; c < count; c++) {
    analogWrite(colorStatus[c].pin, 0);
  }
}
 
void loop() {
  MIDI.read();

  timePassed = millis();
  
  for (byte c = 0; c < count; c++) {
    if (!colorStatus[c].isOn && colorStatus[c].level >= 0) {
      unsigned long decay = timePassed - colorStatus[c].timeStamp;
      double ratio = ((double)decay)/decayTime;
      colorStatus[c].level = colorStatus[c].fullLevel - round(ratio*((double)colorStatus[c].fullLevel));
      analogWrite(colorStatus[c].pin, colorStatus[c].level > 0 ? colorStatus[c].level : 0);
    }
  }
  
}

byte toLevel(byte velocity) {
   return velocity <= 1 ? 0 : velocity*2 + 1;
}

void handleNoteOn(byte channel, byte note, byte velocity) {
  byte level = toLevel(velocity);
  bool isOn = level >= 1;

  Color color;
  
  if (note == REDNOTE) {
    color = red;
  }
  else if (note == GREENNOTE) {
    color = green;
  }
  else if (note == BLUENOTE) {
    color = blue;
  } else {
    return;
  }
  
  analogWrite(colorStatus[color].pin, level);
  colorStatus[color].level = level;
  colorStatus[color].fullLevel = level;
  colorStatus[color].isOn = isOn;
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  Color color;
  if (note == REDNOTE) {
    color = red;
  }
  else if (note == GREENNOTE) {
    color = green;
  }
  else if (note == BLUENOTE) {
    color = blue;
  } else {
    return;
  }
  
  colorStatus[color].isOn = false;
  colorStatus[color].timeStamp = timePassed;
}

void handleControlChange(byte channel, byte control, byte value) {
  if (control == CC) {
    double ratio = ((double)value)/127.0;
    decayTime = 1 + pow(ratio,2) * 3000;
  }
}
