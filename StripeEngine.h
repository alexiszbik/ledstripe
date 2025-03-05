#pragma once

#include "LEDStatus.h"
#include "Color.h"
#include "hsv2rgb.h"

#define REDPIN 9
#define GREENPIN 10
#define BLUEPIN 11

#define REDPIN_B 5
#define GREENPIN_B 6
#define BLUEPIN_B 3

#define NOTE_START 64
#define NOTE_START_B 72

#define REDNOTE_ALL 60

#define RAINBOW_NOTE 58

#define EXPLODE_NOTE 59

enum Mode {
  Normal,
  Rainbow,
  Explode
};

class StripeEngine {
public:

  void setup(bool isSlave) {
    byte noteStart = isSlave ? NOTE_START_B : NOTE_START;
    byte whiteAll = REDNOTE_ALL + 3;

    byte pins[ledCount] = {REDPIN, GREENPIN, BLUEPIN, REDPIN_B, GREENPIN_B, BLUEPIN_B};

    for (int c = 0; c < Color::count; c++) {
      colorStatus[c].init(pins[c], noteStart + c, REDNOTE_ALL + c, noteStart + 3, whiteAll);
      colorStatus[Color::count + c].init(pins[Color::count +c], noteStart + Color::count + 1 + c, REDNOTE_ALL + c, noteStart + 7, whiteAll);
    }
  }

  void loop() {
    if ((millis() - rainbowTS) > rainbowTime) {
      rainbowTS = millis();
      hue = (hue + 2) % 360;
    }

    timePassed = millis();

    switch (mode) {
      case Normal : {
        if (needsToClearColors) {
          for (byte c = 0; c < ledCount; c++) {
            colorStatus[c].clear();
          }
          needsToClearColors = false;
        }
          
        for (byte c = 0; c < ledCount; c++) {
          colorStatus[c].decayAndApply(timePassed, decayTime);
        }
      } break;

      case Rainbow : {
        rgb rgbColor = hsv2rgb({(float)hue,1,1});
        colorStatus[red].level = rgbColor.r * 255;
        colorStatus[green].level = rgbColor.g * 255;
        colorStatus[blue].level = rgbColor.b * 255;

        colorStatus[Color::count + red].level = rgbColor.r * 255;
        colorStatus[Color::count + green].level = rgbColor.g * 255;
        colorStatus[Color::count + blue].level = rgbColor.b * 255;

        for (byte c = 0; c < ledCount; c++) {
          colorStatus[c].applyLevel();
        }

      } break;

      case Explode : {
        if(timePassed - explodeTS > 40) {
          explodeTS = timePassed;

          int chosenLed = random(0, 4);
          int level = random(40, 255);

          for (byte c = 0; c < ledCount; c++) {
            if (chosenLed == (floor(c/Color::count) + (isSlave ? 0 : 2))) {
              colorStatus[c].update(true, level, timePassed);
            } else {
              colorStatus[c].update(false, 0, timePassed);
            }
          }
        }
        for (byte c = 0; c < ledCount; c++) {
          colorStatus[c].decayAndApply(timePassed, decayTime);
        }
      } break;
    }
  }

  void setNote(byte note, byte velocity) {
    byte level = toLevel(velocity);
    bool isOn = level >= 1;

    switch (note) {
      case RAINBOW_NOTE : {
        mode = isOn ? Rainbow : Normal;
        if (!isOn) {
          needsToClearColors = true;
        }
      } break;

      case EXPLODE_NOTE : {
        mode = isOn ? Explode : Normal;
        if (!isOn) {
          needsToClearColors = true;
        }
      } break;

      default : {
        mode = Normal;

        for (byte c = 0; c < ledCount; c++) {
          if (colorStatus[c].note == note || colorStatus[c].noteAll == note || colorStatus[c].noteWhite == note || colorStatus[c].noteWhiteAll == note) {
            colorStatus[c].update(isOn, level, timePassed);
          }
        }

      } break;
    }

  }

  byte toLevel(byte velocity) {
   return velocity <= 1 ? 0 : velocity*2 + 1;
  }

  void setDecayTime(byte byteValue) {
    double ratio = ((double)byteValue)/127.0;
    decayTime = 1 + pow(ratio,2) * 3000;
  }

  void setRainbowTime(byte byteValue) {
    rainbowTime = 1 + byteValue;
  }

  void setExplodeLevel(byte byteValue) {

  }

private:
  bool isSlave = false;
  int hue = 0;
  int explodeLevel = 255;
  unsigned long timePassed = 0;
  unsigned long decayTime = 1;
  unsigned long rainbowTS = 0;
  unsigned long rainbowTime = 1;
  Mode mode = Normal;
  bool needsToClearColors = false;

  unsigned long explodeTS = 0;
  static const int ledCount = Color::count * 2;

  LEDStatus colorStatus[ledCount];

};