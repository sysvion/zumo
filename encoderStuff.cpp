#include "motors.h"
#include "encoderStuff.h"
#include <Zumo32U4.h>

Zumo32U4Encoders encoders;

//CORRECT SPEED VARIABLES:
char correctionValues[100];

int expectedLeftEncoderCount = 0;
int expectedRightEncoderCount = 0;
double offsetLeftEncoderCount = 1;
double offsetRightEncoderCount = 1;
int correctLeft = 0;
int correctRight = 0;
int countsLeft;
int countsRight;
int lastEncodersCheckTime;
const int ALLOWED_SPEED_OFFSET = 20;

void encoderStuff::resetEncoderCounts() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  expectedLeftEncoderCount = 0;
  expectedRightEncoderCount = 0;
}

int encoderStuff::calculateCorrectionStrength(int32_t x) {
  if (x >= 0)
    return sqrt(x * 40);
  else
    return -sqrt(-x * 40);
}

void encoderStuff::correctLeftFaster() {
  correctLeft = calculateCorrectionStrength(offsetLeftEncoderCount);
}

void encoderStuff::correctRightFaster() {
  correctRight = calculateCorrectionStrength(offsetRightEncoderCount);
}

void encoderStuff::correctLeftSlower() {
  correctLeft = calculateCorrectionStrength(offsetLeftEncoderCount);
}
void encoderStuff::correctRightSlower() {
  correctRight = calculateCorrectionStrength(offsetRightEncoderCount);
}

void encoderStuff::correctOffset() {
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
      // Serial.println("correctFaster");
    }
    if (offsetRightEncoderCount > ALLOWED_SPEED_OFFSET) {
      correctRightFaster();
    }

    if (offsetLeftEncoderCount < -ALLOWED_SPEED_OFFSET) {
      correctLeftSlower();
      // Serial.println("correctLeftSlower");
    }
    if (offsetRightEncoderCount < -ALLOWED_SPEED_OFFSET) {
      correctRightSlower();
    }
  }
}

void encoderStuff::setExpectedLeftEncoderCount(int x) {
  expectedLeftEncoderCount = x;
}

void encoderStuff::setExpectedRightEncoderCount(int x) {
  expectedRightEncoderCount = x;
}

int encoderStuff::getCorrectLeft() {
  return correctLeft;
}

int encoderStuff::getCorrectRight() {
  return correctRight;
}

void encoderStuff::printCorrectionValues()
{
  snprintf_P(correctionValues, sizeof(correctionValues), PSTR("%6d %6d exp L: %4d exp R: %4d"),
          countsLeft, countsRight,
          expectedLeftEncoderCount, expectedRightEncoderCount);
  Serial.println((String)correctionValues + "\toffL:" + offsetLeftEncoderCount + "\toffR:" + offsetRightEncoderCount
  + "\tcorrectLeft:" + correctLeft + "\tcorrectRight:" + correctRight
      );
}