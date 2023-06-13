#include <Zumo32U4.h>
#pragma once

void lineSensorsInitFiveSensors();

void CalibrateGreen();

void CalibrateGray();

void CalibrateBrown();

int CalibrateSensors();

int getCalibratedCount();

int readLine(unsigned int *);

bool isGreen(int);

bool isGray(int);

bool isBrown(int);

bool isBlack(int);

void lineFollow();
