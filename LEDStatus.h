#pragma once

struct LEDStatus {
public:
  void init(byte pinA, byte pinB, byte note) {
    this->pinA = pinA;
    this->pinB = pinB;
    this->note = note;

    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);

    analogWrite(pinA, 0);
    analogWrite(pinB, 0);
  }

  void clear() {
    level = -1;
    analogWrite(pinA, 0);
    analogWrite(pinB, 0);
  }

  void applyLevel() {
    analogWrite(pinA, level > 0 ? level : 0);
    analogWrite(pinB, level > 0 ? level : 0);
  }

  void decayAndApply(unsigned long timePassed, unsigned long decayTime) {
    if (!isOn && level >= 0) {
      unsigned long decay = timePassed - timeStamp;
      double ratio = ((double)decay)/decayTime;
      level = fullLevel - round(ratio*((double)fullLevel));
      applyLevel();
    }
  }

public:
  byte pinA;
  byte pinB;
  byte note;

  int level = 0;
  byte fullLevel = 0;
  bool isOn = false;
  unsigned long timeStamp = 0;
};