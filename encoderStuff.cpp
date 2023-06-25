#include "USBAPI.h"
#include "motors.h"
#include "encoderStuff.h"
#include <Zumo32U4.h>

#define debugEncoders

void encoderStuff::resetEncoderCounts() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  expectedLeftEncoderCount = 0;
  expectedRightEncoderCount = 0;

  #ifdef debugEncoders
    Serial.println("reset encoders");
  #endif
}

const int encoderStuff::calculateCorrectionStrength(int32_t x) {
  if (x >= 0)
    return sqrt(x * 40);
  else
    return -sqrt(-x * 40);
}


void encoderStuff::correctOffset() {
  if (!((uint8_t)(millis() - lastEncodersCheckTime) >= 50)) {return;}
  lastEncodersCheckTime = millis();

  countsLeft = encoders.getCountsLeft();
  countsRight = encoders.getCountsRight();

  expectedLeftEncoderCount += 0.6 * getSpeedLeft();
  expectedRightEncoderCount += 0.6 * getSpeedRight();

  offsetLeftEncoderCount = expectedLeftEncoderCount - countsLeft;
  offsetRightEncoderCount = expectedRightEncoderCount - countsRight;

  if (offsetLeftEncoderCount < -ALLOWED_SPEED_OFFSET ||
      offsetLeftEncoderCount > ALLOWED_SPEED_OFFSET)
  {
    correctLeft = calculateCorrectionStrength(offsetLeftEncoderCount);
  }

  if (offsetRightEncoderCount < -ALLOWED_SPEED_OFFSET ||
      offsetRightEncoderCount > ALLOWED_SPEED_OFFSET)
  {
    correctRight = calculateCorrectionStrength(offsetRightEncoderCount);
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