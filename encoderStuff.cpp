#include "encoderStuff.h"
#include <Zumo32U4.h>
#include <math.h>

#define ALLOWED_SPEED_OFFSET 20

void encoderStuff::resetEncoderCounts() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  expectedLeftEncoderCount = 0;
  expectedRightEncoderCount = 0;
}

// old name: calculateCorrectStrengt(int x)
int encoderStuff::NormaliseStrength(int32_t Strength) {
  if (Strength >= 0)
    return sqrt(Strength * 40);
  else
    return -sqrt(-Strength * 40);
}

void encoderStuff::correctOffset(int L, int R) {
  if ((uint8_t)(millis() - lastEncodersCheckTime) >= 50) {
    lastEncodersCheckTime = millis();

    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();

    expectedLeftEncoderCount += 0.6 * L;
    expectedRightEncoderCount += 0.6 * R;

    offsetLeftEncoderCount = expectedLeftEncoderCount - countsLeft;
    offsetRightEncoderCount = expectedRightEncoderCount - countsRight;

   
    if (offsetLeftEncoderCount > ALLOWED_SPEED_OFFSET ||
        offsetLeftEncoderCount < -ALLOWED_SPEED_OFFSET ) {
      correctLeft = NormaliseStrength(offsetLeftEncoderCount);
    }

    if (offsetRightEncoderCount > ALLOWED_SPEED_OFFSET ||
        offsetRightEncoderCount < -ALLOWED_SPEED_OFFSET) {
      correctRight = NormaliseStrength(offsetRightEncoderCount);
    }

  }
}

void encoderStuff::setExpectedLeftEncoderCount(int value) {
  expectedLeftEncoderCount = value;
}

void encoderStuff::setExpectedRightEncoderCount(int value) {
  expectedRightEncoderCount = value;
}

int encoderStuff::getCorrectLeft() {
  return correctLeft;
}

int encoderStuff::getCorrectRight() {
  return correctRight;
}

void encoderStuff::printCorrectionValues()
{
  //CORRECT SPEED VARIABLES:
  char correctionValues[100];
  snprintf_P(correctionValues, sizeof(correctionValues), PSTR("%6d %6d exp L: %4d exp R: %4d"),
          countsLeft, countsRight,
          expectedLeftEncoderCount, expectedRightEncoderCount);
  Serial.println((String)correctionValues + "\toffL:" + offsetLeftEncoderCount + "\toffR:" + offsetRightEncoderCount
  + "\tcorrectLeft:" + correctLeft + "\tcorrectRight:" + correctRight
      );
}
