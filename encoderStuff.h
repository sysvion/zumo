#include <Zumo32U4.h>
#pragma once

void resetEncoderCounts();

int calculateCorrectionStrength(int32_t);

void correctLeftFaster();

void correctRightFaster();

void correctLeftSlower();

void correctRightSlower();

void correctOffset();

void setExpectedLeftEncoderCount(int);

void setExpectedRightEncoderCount(int);

int getCorrectLeft();

int getCorrectRight();

void printCorrectionValues();