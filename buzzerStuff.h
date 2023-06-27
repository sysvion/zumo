#include <Zumo32U4.h>
#pragma once

class buzzerStuff {

public:
  void play(int, int);
  void lowerVolume();
  void increaseVolume();
  int getVolume();

  void startupSound();
  void scanColorSound();
  void autonomousModeSound();
  void manualModeSound();
  int playSoundById(int);

  int playSoundId = 0;
  const int MIN_VOLUME = 6;
  const int MAX_VOLUME = 13;

private:
  int volume = MIN_VOLUME + 4;
  Zumo32U4Buzzer buzzer;
};
