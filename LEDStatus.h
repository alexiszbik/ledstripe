#pragma once

struct LEDStatus {
public:
  void init(byte pin, byte note, byte noteAll, byte noteWhite, byte noteWhiteAll) {
    this->pin = pin;
    this->note = note;
    this->noteAll = noteAll;
    this->noteWhite = noteWhite;
    this->noteWhiteAll = noteWhiteAll;

    pinMode(pin, OUTPUT);

    analogWrite(pin, 0);
  }

  void clear() {
    level = -1;
    analogWrite(pin, 0);
  }

  void applyLevel() {
    analogWrite(pin, level > 0 ? level : 0);
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
  byte pin;
  byte note;
  byte noteAll;
  byte noteWhite;
  byte noteWhiteAll;

  int level = 0;
  byte fullLevel = 0;
  bool isOn = false;
  unsigned long timeStamp = 0;
};