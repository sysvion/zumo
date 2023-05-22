#include <Wire.h>
#include <Zumo32U4.h>
#include <math.h>
#include "test.h"
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
Zumo32U4ButtonC buttonC;

char inputChar;


bool isDriving = true;            //determines if the robot should move or pause

const int MIN_VOLUME = 6;
const int MAX_VOLUME = 13;
int volume = MIN_VOLUME;

int count = 0;

void sendManualToPc() {
  tester::test();
  Serial1.println("\n\nZUMO MANUAL MODE\n  WASD to move\n  SPACE to stop/continue\n  Q to mote at max speed\n  E to reset rotational movement\n  R to reset movement\n -/+ keys to change volume\n");
}

void setup()
{
  Serial.begin(9600);         //start serial connection with the Arduino serial
  Serial1.begin(9600);        //start serial connection with the XCTU application (Xbee serial)

  // why not !Serial1
  while (!Serial/*isReady()*/) {}

  sendManualToPc();
}

//function to simplify/shorten the use of buzzer in the main loop
void playFrequency(int frequency, int durationMilliseconds)
{
  buzzer.playFrequency(frequency, durationMilliseconds, volume);
}

void stopContinue()
{
  if (isDriving)
  {
    isDriving = false;
    playFrequency(300, 80);
    delay(120);
    playFrequency(280, 80);
    return;
  }
  else
  {
    isDriving = true;           //starts the robot
    playFrequency(280, 80);
    delay(120);
    playFrequency(300, 80);
  }
}

void lowerVolume()
{
                                    // is this true? if it is already at the maximum i can't lower the volume
  if (volume > MIN_VOLUME)          //checks if volume is already at maximum
  {
    volume -= 1;
    Serial1.println((String)"Volume: " + (volume-6));
    playFrequency(460, 300);
  }
}

void increaseVolume()
{
  if (volume < MAX_VOLUME)          //checks if volume is already at minimum
  {
    volume += 1;
    Serial1.println((String)"Volume: " + (volume-6));
    playFrequency(460, 300);
  }
}

void resetRotationalMovement()
{ 
  //resets steer values but not the overal speed so the robots starts driving straight forward
  steerLeft = 1;
  steerRight = 1;
  Serial1.println("RESET ROTATION");
  ledYellow(1);                     //blinks the yellow led 3 times
  playFrequency(280, 80);
  delay(50);
  ledYellow(0);
  delay(50);
  ledYellow(1);
  playFrequency(300, 80);
  delay(100);
  ledYellow(0);
  playFrequency(280, 80);
}

void rotateDeg(int deg)
{
  double rotateSpeed = speed * 40;            //set rotateSpeed to motor speed

  if (speed <= 2) 
  {
      rotateSpeed = 120;
  }

  //the robot will make a longer turn if it is moving slower.
  //To account for more resistance at lower speeds
  //1st num in pow: smaller = shorter turn.
  //2nd num in pow: higher = longer turn for lower speeds
  deg /= pow(rotateSpeed/600, 1.7);            

  if (rotateSpeed > 400) rotateSpeed = 400;   //speed cant exceed 400
  if (deg < 0) rotateSpeed *= -1;

  motors.setLeftSpeed(-rotateSpeed);
  motors.setRightSpeed(rotateSpeed*1.1);
  if (deg < 0) deg *= -1;                     //if deg is negative, make sure that the delay isn't a negative number
  delay(deg);
  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);
  steerLeft = 1;
  steerRight = 1;
}

void wait()
{
  playFrequency(200, 250);
  delay(250);
}

void loop()
{   //when button C is pressed, message how to use the control keys is printed into Serial1 again
  if (buttonC.isPressed()) {
      sendManualToPc();
  }

  /*if (Serial.available())         //this prints Serial input into Serial1
  { 
    Serial1.write(Serial.read());
  }*/
  if (Serial1.available())          //if character is received form Serial1
  { 
    inputChar = Serial1.read();     //stores received character into a char varible

    switch (inputChar)              //a switch is used instead of multiple if-else statements to make code more readable and optimized
    {

      case '-':                     //if input char is "-", call lowerVolume() function. The same is done for every other key which call other functions
        lowerVolume();
        break;                      //break out of switch

      case '=':
        increaseVolume();
        break;

      case 'a':
        moveLeft();
        break;

      case 'd':
        moveRight();
        break;

      case 's':
        moveSlower();
        break;

      case 'w':
        moveFaster();
        break;

      case 'q':
        moveToMaxSpeed();
        break;

      case 'R':
      case 'r':
        resetSpeed();
        break;

      case 'e':
        resetRotationalMovement();
        break;

      case 'A':
        rotateDeg(90);
        break;

      case 'D':
        rotateDeg(-90);
        break;

      case 'S':
        rotateDeg(-180);
        break;

      case '0':
        wait();
        break;

      default:              //"default:" is ran if none of other cases were activated. This is needed for the ' ' character (SPACEBAR) because this gives an error in a regular case.
        //stop/continue moving
        if (inputChar == ' ') stopContinue(); 
        break;
    }

    //these two formulas determine the final speeds for the left and right motor
    speedLeft  = (speed * steerLeft - steerRight) * 50;
    speedRight = (speed * steerRight - steerLeft) * 50 * 1.04;

    //these statements set the motor speeds to the minimum or maximum allowed value if these are above or below allowed vaues
    if (speedLeft  > MAX_SPEED) speedLeft = MAX_SPEED;
    if (speedRight > MAX_SPEED) speedRight = MAX_SPEED;
    if (speedLeft  < MIN_SPEED) speedLeft = MIN_SPEED;
    if (speedRight < MIN_SPEED) speedRight = MIN_SPEED;

    //prints the left/right motor speed and the two steer values
    Serial1.println((String)"\nLEFT: " + speedLeft + "  steerLeft: " + steerLeft + "\nRIGHT: " + speedRight + "  steerRight: " + steerRight);

    if (!isDriving)
    {
      speedLeft = 0;      //sets the motor speeds to 0 to stop them. When robot is allowed to drive again, it will continue at the last set speed instead of resetting its speed.
      speedRight = 0;
      Serial1.println("STOPPED");
    }

    motors.setLeftSpeed(speedLeft);
    motors.setRightSpeed(speedRight);
  }

  if (!isDriving)
  {
    count++;
    if (count > 5)
    {
      playFrequency(200, 40);
      count = 0;
    }

    ledRed(1);
    delay(50);
    ledRed(0);
    delay(100);
  }

  if (speedLeft == 0 && speedRight == 0)  //checks if both motors are not moving
  {
    ledGreen(0);
  }
  else
  {
    ledGreen(1);
  }
}
