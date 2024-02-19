#pragma once

struct LEDStatus {
  int level = 0;
  byte fullLevel = 0;
  bool isOn = false;
  unsigned long timeStamp = 0;
  byte pinA;
  byte pinB;
};