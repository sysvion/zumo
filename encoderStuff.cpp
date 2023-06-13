#include "motors.h"
#include "encoderStuff.h"
#include <Zumo32U4.h>

Zumo32U4Encoders encoders;


int expectedLeftEncoderCount = 0;
int expectedRightEncoderCount = 0;
int correctLeft = 0;
int correctRight = 0;
int lastEncodersCheckTime = 0;
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

void correctOffset()
{
  if ((uint8_t)(millis() - lastEncodersCheckTime) >= 50) { return; }
  lastEncodersCheckTime = millis();

  const int countsLeft = encoders.getCountsLeft();
  const int countsRight = encoders.getCountsRight();

  expectedLeftEncoderCount  += 0.6 * /*motor::*/speedLeft;
  expectedRightEncoderCount += 0.6 * /*motor::*/speedRight;

  const int offsetLeftEncoderCount = expectedLeftEncoderCount - countsLeft;
  const int offsetRightEncoderCount = expectedRightEncoderCount - countsRight;

  if (offsetLeftEncoderCount >  ALLOWED_SPEED_OFFSET ||
      offsetLeftEncoderCount < -ALLOWED_SPEED_OFFSET) 
  {
    correctLeft = calculateCorrectionStrength(offsetLeftEncoderCount);
    #ifdef debugEncoder
    Serial.println("correctFaster");
    #endif
  }

  if (offsetRightEncoderCount > ALLOWED_SPEED_OFFSET ||
      offsetRightEncoderCount < -ALLOWED_SPEED_OFFSET)
  {
    correctRight = calculateCorrectionStrength(offsetRightEncoderCount);
    #ifdef debugEncoder
    Serial.println("correctLeft");
    #endif
  }
}


void setExpectedLeftEncoderCount(const int x) {
  expectedLeftEncoderCount = x;
}

void setExpectedRightEncoderCount(const int x) {
  expectedRightEncoderCount = x;
}

const int getCorrectLeft() {
  return correctLeft;
}

const int getCorrectRight() {
  return correctRight;
}