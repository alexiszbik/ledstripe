// color swirl! connect an RGB LED to the PWM pins as indicated
// in the #defines
// public domain, enjoy!

#include <MIDI.h>
#include <Wire.h>
#include "LEDStatus.h"

#define MIDI_CHANNEL 13
#define CC_SPEED 3
#define CC_RAINBOWSPEED 4

MIDI_CREATE_DEFAULT_INSTANCE();
 
#define REDPIN 9
#define GREENPIN 10
#define BLUEPIN 11

#define REDPIN_B 5
#define GREENPIN_B 6
#define BLUEPIN_B 3

#define REDNOTE 60
#define GREENNOTE 61
#define BLUENOTE 62
#define RAINBOWNOTE 63

unsigned long decayTime = 1;
unsigned long rainbowTS = 0;
unsigned long rainbowTime = 1;
bool rainbowStatus = false;
bool needsToClearColors = false;

typedef struct {
    float r;       // a fraction between 0 and 1
    float g;       // a fraction between 0 and 1
    float b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    float h;       // angle in degrees
    float s;       // a fraction between 0 and 1
    float v;       // a fraction between 0 and 1
} hsv;

rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}

enum Color {
  red = 0,
  green,
  blue,

  count
};

LEDStatus colorStatus[count];
unsigned long timePassed;
 
void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  colorStatus[red].pinA = REDPIN;
  colorStatus[green].pinA = GREENPIN;
  colorStatus[blue].pinA = BLUEPIN;

  colorStatus[red].pinB = REDPIN_B;
  colorStatus[green].pinB = GREENPIN_B;
  colorStatus[blue].pinB = BLUEPIN_B;

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleControlChange);
  MIDI.begin(MIDI_CHANNEL);

  for (byte c = 0; c < count; c++) {
    analogWrite(colorStatus[c].pinA, 0);
    analogWrite(colorStatus[c].pinB, 0);
  }

  Wire.begin(); 
}

int hue = 0;

void loop() {
  MIDI.read();

  if ((millis() - rainbowTS) > rainbowTime) {
    rainbowTS = millis();
    hue = (hue + 2) % 360;
  }

  timePassed = millis();

  if (rainbowStatus) {
    rgb rgbColor = hsv2rgb({hue,1,1});
    colorStatus[0].level = rgbColor.r * 255;
    colorStatus[1].level = rgbColor.g * 255;
    colorStatus[2].level = rgbColor.b * 255;

    for (byte c = 0; c < count; c++) {
      analogWrite(colorStatus[c].pinA, colorStatus[c].level > 0 ? colorStatus[c].level : 0);
    }
  } else {
    if (needsToClearColors) {
      colorStatus[0].level = -1;
      colorStatus[1].level = -1;
      colorStatus[2].level = -1;
      for (byte c = 0; c < count; c++) {
        analogWrite(colorStatus[c].pinA, 0);
        analogWrite(colorStatus[c].pinB, 0);
      }
      needsToClearColors = false;
    }
      
    for (byte c = 0; c < count; c++) {
      if (!colorStatus[c].isOn && colorStatus[c].level >= 0) {
        unsigned long decay = timePassed - colorStatus[c].timeStamp;
        double ratio = ((double)decay)/decayTime;
        colorStatus[c].level = colorStatus[c].fullLevel - round(ratio*((double)colorStatus[c].fullLevel));
        analogWrite(colorStatus[c].pinA, colorStatus[c].level > 0 ? colorStatus[c].level : 0);
        analogWrite(colorStatus[c].pinB, colorStatus[c].level > 0 ? colorStatus[c].level : 0);
      }
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
    if (note == RAINBOWNOTE) {
      rainbowStatus = true;
    }
    return;
  }
  
  analogWrite(colorStatus[color].pinA, level);
  analogWrite(colorStatus[color].pinB, level);
  colorStatus[color].level = level;
  colorStatus[color].fullLevel = level;
  colorStatus[color].isOn = isOn;

  Wire.beginTransmission(9);
  byte buffer[2];
  buffer[0] = note;
  buffer[1] = 127;
  Wire.write( buffer, 2);
  Wire.endTransmission(); 
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
    if (note == RAINBOWNOTE) {
      rainbowStatus = false;
      needsToClearColors = true;
    }
    return;
  }
  
  colorStatus[color].isOn = false;
  colorStatus[color].timeStamp = timePassed;

  Wire.beginTransmission(9);
  byte buffer[2];
  buffer[0] = note;
  buffer[1] = 0;
  Wire.write(buffer, 2);
  Wire.endTransmission(); 
}

void handleControlChange(byte channel, byte control, byte value) {
  if (control == CC_SPEED) {
    double ratio = ((double)value)/127.0;
    decayTime = 1 + pow(ratio,2) * 3000;
  }
  if (control == CC_RAINBOWSPEED) {
    rainbowTime = 1 + value;
  }
}
