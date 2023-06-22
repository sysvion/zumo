#include <Zumo32U4.h>
#pragma once
extern int playSoundId;

void play(int, int);
void lowerVolume();
void increaseVolume();
void startupSound();
void scanColorSound();
void autonomousModeSound();
void manualModeSound();
int playSoundById(int);