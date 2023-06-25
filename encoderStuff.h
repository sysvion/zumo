#include <Zumo32U4.h>
#pragma once

class encoderStuff {

public:
    /// main method that updates the offset of the motors 
    void correctOffset();
    void resetEncoderCounts();

    void setExpectedLeftEncoderCount(int);
    void setExpectedRightEncoderCount(int);

    ///getter for the motor methods
    int getCorrectLeft();

    ///getter for the motor methods
    int getCorrectRight();

    //debug info
    void printCorrectionValues();

private:
    const int calculateCorrectionStrength(int32_t);

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

    Zumo32U4Encoders encoders;

};