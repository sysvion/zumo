#include "motors.h"
#include "encoderStuff.h"
#include <Zumo32U4.h>

//#define debugEncoder
Zumo32U4Encoders encoders;

//CORRECT SPEED VARIABLES:
char correctionValues[100];

int expectedLeftEncoderCount = 0;
int expectedRightEncoderCount = 0;
double expectedOffsetLeftEncoderCount = 1;
double expectedOffsetRightEncoderCount = 1;
double offsetLeftEncoderCount = 1;
double offsetRightEncoderCount = 1;
int correctLeft = 0;
int correctRight = 0;
int countsLeft;
int countsRight;
int lastEncodersCheckTime;
const int ALLOWED_SPEED_OFFSET = 20;

void resetEncoderCounts()
{
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  expectedLeftEncoderCount = 0;
  expectedRightEncoderCount = 0;
}

int calculateCorrectionStrength(int32_t x)
{
  if (x >= 0)
    return sqrt(x * 40);
  else
    return -sqrt(-x * 40);
}

void correctLeftFaster()
{
  correctLeft = calculateCorrectionStrength(offsetLeftEncoderCount);
}

void correctRightFaster()
{
}

void correctLeftSlower()
{
  correctLeft = calculateCorrectionStrength(offsetLeftEncoderCount);
}
void correctRightSlower()
{
  correctRight = calculateCorrectionStrength(offsetRightEncoderCount);
}

void correctOffset()
{
  if ((uint8_t)(millis() - lastEncodersCheckTime) >= 50) {
    lastEncodersCheckTime = millis();

    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();

    expectedLeftEncoderCount += 0.6 * getSpeedLeft();
    expectedRightEncoderCount += 0.6 * getSpeedRight();

    offsetLeftEncoderCount = expectedLeftEncoderCount - countsLeft;
    offsetRightEncoderCount = expectedRightEncoderCount - countsRight;

    if (offsetLeftEncoderCount > ALLOWED_SPEED_OFFSET) {
      correctLeftFaster();
      #ifdef debugEncoder
      Serial.println("correctFaster");
      #endif
    }
    if (offsetRightEncoderCount > ALLOWED_SPEED_OFFSET) {
      correctRightFaster();
    }

    if (offsetLeftEncoderCount < -ALLOWED_SPEED_OFFSET) {
      correctLeftSlower();
        #ifdef debugEncoder
        Serial.println("correctLeftSlower");
        #endif
    }
    if (offsetRightEncoderCount < -ALLOWED_SPEED_OFFSET) {
      correctRightSlower();
    }
  }
}


void setExpectedLeftEncoderCount(int x) {
  expectedLeftEncoderCount = x;
}

void setExpectedRightEncoderCount(int x) {
  expectedRightEncoderCount = x;
}

int getCorrectLeft() {
  return correctLeft;
}

int getCorrectRight() {
  return correctRight;
}