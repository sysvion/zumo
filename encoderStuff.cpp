#include "motors.h"
#include "encoderStuff.h"
#include <Zumo32U4.h>

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

void correctRightSlower()
{
}

void correctOffset()
{
  if ((uint8_t)(millis() - lastEncodersCheckTime) >= 50) {
    lastEncodersCheckTime = millis();

    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();

    expectedLeftEncoderCount += 0.6 * speedLeft;
    expectedRightEncoderCount += 0.6 * speedRight;

    offsetLeftEncoderCount = expectedLeftEncoderCount - countsLeft;
    offsetRightEncoderCount = expectedRightEncoderCount - countsRight;

    if (offsetLeftEncoderCount > ALLOWED_SPEED_OFFSET) {
      correctLeft = calculateCorrectionStrength(offsetLeftEncoderCount);
      #ifdef debugEncoder
      Serial.println("correctFaster");
      #endif
    }
    if (offsetRightEncoderCount > ALLOWED_SPEED_OFFSET) {
      correctRight = calculateCorrectionStrength(offsetRightEncoderCount);
    }

    if (offsetLeftEncoderCount < -ALLOWED_SPEED_OFFSET) {
      correctLeft = calculateCorrectionStrength(offsetLeftEncoderCount);
        #ifdef debugEncoder
        Serial.println("correctLeftSlower 1");
        #endif
    }
    if (offsetRightEncoderCount < -ALLOWED_SPEED_OFFSET) {
        correctRight = calculateCorrectionStrength(offsetRightEncoderCount);
        #ifdef debugEncoder
        Serial.println("correctLeftSlower 1");
        #endif
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