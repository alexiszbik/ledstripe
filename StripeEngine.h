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

#define REDNOTE_ALL 60

#define RAINBOWNOTE 58

class StripeEngine {
public:

  void setup(byte redStart) {
    byte whiteAll = REDNOTE_ALL + 3;
    colorStatus[red].init(REDPIN, redStart, REDNOTE_ALL, redStart + 3, whiteAll);
    colorStatus[green].init(GREENPIN, redStart + 1, REDNOTE_ALL + 1, redStart + 3, whiteAll);
    colorStatus[blue].init(BLUEPIN, redStart + 2, REDNOTE_ALL + 2, redStart + 3, whiteAll);

    colorStatus[Color::count + red].init(REDPIN_B, redStart + 4, REDNOTE_ALL, redStart + 7, whiteAll);
    colorStatus[Color::count + green].init(GREENPIN_B, redStart + 5, REDNOTE_ALL + 1, redStart + 7, whiteAll);
    colorStatus[Color::count + blue].init(BLUEPIN_B, redStart + 6, REDNOTE_ALL + 2, redStart + 7, whiteAll);
  }

  void loop() {
    if ((millis() - rainbowTS) > rainbowTime) {
      rainbowTS = millis();
      hue = (hue + 2) % 360;
    }

    timePassed = millis();

    if (rainbowStatus) {
      rgb rgbColor = hsv2rgb({hue,1,1});
      colorStatus[red].level = rgbColor.r * 255;
      colorStatus[green].level = rgbColor.g * 255;
      colorStatus[blue].level = rgbColor.b * 255;

      colorStatus[Color::count + red].level = rgbColor.r * 255;
      colorStatus[Color::count + green].level = rgbColor.g * 255;
      colorStatus[Color::count + blue].level = rgbColor.b * 255;

      for (byte c = 0; c < ledCount; c++) {
        colorStatus[c].applyLevel();
      }
    } else {
      if (needsToClearColors) {
        for (byte c = 0; c < ledCount; c++) {
          colorStatus[c].clear();
        }
        needsToClearColors = false;
      }
        
      for (byte c = 0; c < ledCount; c++) {
        colorStatus[c].decayAndApply(timePassed, decayTime);
      }
    }
  }


  void setColor(byte note, byte velocity) {
    byte level = toLevel(velocity);
    bool isOn = level >= 1;

    if (note == RAINBOWNOTE) {
      rainbowStatus = isOn;
      if (!isOn) {
        needsToClearColors = true;
      }
      return;
    } else {
       for (byte c = 0; c < ledCount; c++) {
          if (colorStatus[c].note == note || colorStatus[c].noteAll == note || colorStatus[c].noteWhite == note || colorStatus[c].noteWhiteAll == note) {
            colorStatus[c].isOn = isOn;
            if (isOn) {
              colorStatus[c].level = level;
              colorStatus[c].fullLevel = level;
            } else {
              colorStatus[c].timeStamp = timePassed;
            }
            colorStatus[c].applyLevel();
          }
       }
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

private:
  int hue = 0;
  unsigned long timePassed;
  unsigned long decayTime = 1;
  unsigned long rainbowTS = 0;
  unsigned long rainbowTime = 1;
  bool rainbowStatus = false;
  bool needsToClearColors = false;

  static const int ledCount = Color::count * 2;

  LEDStatus colorStatus[ledCount];

};