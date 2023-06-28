#pragma once
#include <Zumo32U4.h>

class encoderStuff {

public:
  void resetEncoderCounts();

  void correctOffset(int, int);

  /// for movement turning amount of degrees
  void setExpectedLeftEncoderCount(int);

  /// for movement turning precise amount of degrees
  void setExpectedRightEncoderCount(int);

  int getCorrectLeft();
  int getCorrectRight();

  /// debug method
  void printCorrectionValues();

private:
  Zumo32U4Encoders encoders;

  /// old name: calculateCorrectStrengt(int x)
  ///
  /// make motor strength more realistic
  int NormaliseStrength(int32_t);

  int expectedLeftEncoderCount = 0;
  int expectedRightEncoderCount = 0;
  double offsetLeftEncoderCount = 1;
  double offsetRightEncoderCount = 1;
  int correctLeft = 0;
  int correctRight = 0;
  int countsLeft = 0;
  int countsRight = 0;
  int lastEncodersCheckTime = 0;

  const int ALLOWED_SPEED_OFFSET = 20;
};
