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

#define REDNOTE 60
#define GREENNOTE 61
#define BLUENOTE 62

#define RAINBOWNOTE 63

class StripeEngine {
public:

  void setup() {
    colorStatus[red].init(REDPIN, REDPIN_B, REDNOTE);
    colorStatus[green].init(GREENPIN, GREENPIN_B, GREENNOTE);
    colorStatus[blue].init(BLUEPIN, BLUEPIN_B, BLUENOTE);
  }

  void loop() {
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

      for (byte c = 0; c < Color::count; c++) {
        colorStatus[c].applyLevel();
      }
    } else {
      if (needsToClearColors) {
        for (byte c = 0; c < Color::count; c++) {
          colorStatus[c].clear();
        }
        needsToClearColors = false;
      }
        
      for (byte c = 0; c < Color::count; c++) {
        if (!colorStatus[c].isOn && colorStatus[c].level >= 0) {
          unsigned long decay = timePassed - colorStatus[c].timeStamp;
          double ratio = ((double)decay)/decayTime;
          colorStatus[c].level = colorStatus[c].fullLevel - round(ratio*((double)colorStatus[c].fullLevel));
          colorStatus[c].applyLevel();
        }
      }
    }
  }


  void setColor(byte note, byte velocity) {
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
        rainbowStatus = isOn;
        if (!isOn) {
          needsToClearColors = true;
        }
      }
      return;
    }

    colorStatus[color].isOn = isOn;

    if (isOn) {
      colorStatus[color].level = level;
      colorStatus[color].fullLevel = level;
    } else {
      colorStatus[color].timeStamp = timePassed;
    }

    colorStatus[color].applyLevel();
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

  LEDStatus colorStatus[Color::count];

};