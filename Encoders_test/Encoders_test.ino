#include <Wire.h>
#include <Zumo32U4.h>
#include <math.h>

Zumo32U4Encoders encoders;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;

char correctionValues[100];

int count = 0;

int leftSpeed = 0;
int rightSpeed = 0;
int expectedLeftEncoderCount = 0;
int expectedRightEncoderCount = 0;

double expectedOffsetLeftEncoderCount = 1;
double expectedOffsetRightEncoderCount = 1;
double offsetLeftEncoderCount = 1;
double offsetRightEncoderCount = 1;


int correctLeft = 0;
int correctRight = 0;

const int ALLOWED_SPEED_OFFSET = 20;

void setup()
{
  Serial1.begin(9600);
}

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
  correctRight = calculateCorrectionStrength(offsetRightEncoderCount);
}

void correctLeftSlower()
{
  correctLeft = calculateCorrectionStrength(offsetLeftEncoderCount);
}

void correctRightSlower()
{
  correctRight = calculateCorrectionStrength(offsetRightEncoderCount);
}

void printCorrectionValues()
{
  snprintf_P(report, sizeof(report), PSTR("%6d %6d exp L: %6d exp R: %6d"), 
          countsLeft, countsRight, 
          expectedLeftEncoderCount, expectedRightEncoderCount);
  Serial.println((String)report + "   " + offsetLeftEncoderCount + "  " + offsetRightEncoderCount 
  + "   " + correctLeft + "  " + correctRight
      );
  Serial.println((String)"correctLeft " + correctLeft);
  Serial.println((String)"correctRight " + correctRight);
}

void loop()
{
  static uint8_t lastEncodersCheckTime;

  if ((uint8_t)(millis() - lastEncodersCheckTime) >= 50)
  {
    lastEncodersCheckTime = millis();

    int countsLeft = encoders.getCountsLeft();
    int countsRight = encoders.getCountsRight();

    printCorrectionValues();

    expectedLeftEncoderCount  += 0.6 * leftSpeed;
    expectedRightEncoderCount += 0.6 * rightSpeed;
    
    offsetLeftEncoderCount = expectedLeftEncoderCount - countsLeft;
    offsetRightEncoderCount = expectedRightEncoderCount - countsRight;

    if (offsetLeftEncoderCount > ALLOWED_SPEED_OFFSET)
    {
      correctLeftFaster();
    }
    if (offsetRightEncoderCount > ALLOWED_SPEED_OFFSET)
    {
      correctRightFaster();
    }

    if (offsetLeftEncoderCount < -ALLOWED_SPEED_OFFSET)
    {
      correctLeftSlower();
    }
    if (offsetRightEncoderCount < -ALLOWED_SPEED_OFFSET)
    {
      correctRightSlower();
    }
    
    
    
    
  }


  motors.setSpeeds(leftSpeed + correctLeft, rightSpeed + correctRight);


    if (Serial1.available())
  {
    switch (Serial1.read()) {
      case 'q':
        leftSpeed += 80;
        rightSpeed += 80;
        resetEncoderCounts();
        break;

      case 'w':
        leftSpeed = 40;
        rightSpeed = 40;
        resetEncoderCounts();
        break;

      case 'a':
        leftSpeed = 0;
        rightSpeed = 200;
        resetEncoderCounts();
        break;

      case 'd':
        leftSpeed = 30;
        rightSpeed = -30;
        resetEncoderCounts();
        break;

      case 's':
        leftSpeed = 0;
        rightSpeed = 0;
        resetEncoderCounts();
        break;
    }
  }

}
