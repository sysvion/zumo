#include <Zumo32U4.h>
#pragma once

void resetEncoderCounts();

int calculateCorrectionStrength(int32_t);

void correctOffset();

void setExpectedLeftEncoderCount(int);
void setExpectedRightEncoderCount(int);

const int getCorrectLeft();
const int getCorrectRight();