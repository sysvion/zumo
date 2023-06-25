#include <Zumo32U4.h>
#pragma once

class buzzerStuff {

  public:
    void play(int, int);
    void lowerVolume();
    void increaseVolume();
    void startupSound();
    void scanColorSound();
    void autonomousModeSound();
    void manualModeSound();
    int playSoundById(int);

  private:
    extern int playSoundId;
}