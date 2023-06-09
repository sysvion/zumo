#include <Zumo32U4.h>
#pragma once

void lineSensorsInitFiveSensors();

int CalibrateSensors();

int getCalibratedCount();

int readLine(unsigned int *);

void lineFollow();
