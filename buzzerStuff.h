#include <Zumo32U4.h>
#pragma once


    void play(int, int);
    void lowerVolume();
    void increaseVolume();
    void startupSound();
    void scanColorSound();
    void autonomousModeSound();
    void manualModeSound();
    int playSoundById(int);
    extern int playSoundId;