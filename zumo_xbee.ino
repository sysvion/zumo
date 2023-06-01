#include <Wire.h>
#include <Zumo32U4.h>
#include <math.h>
#include "inertial.h"
#include "motors.h"
#include "notify.h"

Zumo32U4ButtonC buttonC;
char inputChar;

bool isDebuging = false;
uint8_t whatToDebug = 0;
int count = 0;

inertial inu = inertial();

//input keys manual
void sendManualToPc() {
  Serial1.println("\n\nZUMO MANUAL MODE\n  WASD to move\n  SPACE to stop/continue\n  Q to mote at max speed\n  E to reset rotational movement\n  R to reset movement\n -/+ keys to change volume\n");
}

void setup()
{
  Serial.begin(9600);         //start serial connection with the Arduino serial
  Serial1.begin(9600);        //start serial connection with the XCTU application (Xbee serial)

 // inu.setup();
  while (!Serial1) {}
  sendManualToPc();
}
//function to simplify/shorten the use of buzzer in the main loop



void loop()
{   //when button C is pressed, message how to use the control keys is printed into Serial1 again
    if (buttonC.isPressed())
    {
        sendManualToPc();
    }

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

            case 'k':
                isDebuging = !isDebuging;
                break;

            case 'j':
                whatToDebug = whatToDebug + 1 % 4;
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

            case 'X':
                rotateDeg(360);
                break;

            default:  //"default:" is ran if none of other cases were activated. This is needed for the ' ' character (SPACEBAR) because this gives an error in a regular case.
                if (inputChar == ' ')
                {
                    stopContinue();
                    makeSomeNoice(100);
                }
                break;
        }


        //these statements set the motor speeds to the minimum or maximum allowed value if these are above or below allowed vaues
       setAndNormalizeMotorValues();
    }

    if (!isAllowDrive())
    {
        count++;
        if (count > 5)
        {
            play(200, 40);
            count = 0;
        }
        ledRed(1);
        delay(50);
        ledRed(0);
        delay(100);
    }

    if (isStandingStill())
    {
      ledGreen(0);
    }
    else
    {
      ledGreen(1);
    }


    if (isDebuging) {
        if (whatToDebug == 0) {
            if (whatToDebug == 0)
            {
                int gyroinfo[3];
                int *value(gyroinfo);
                inu.getGyroPoss(value);
                Serial1.print( value[0] );
                Serial1.print("\t");
                Serial1.print( value[1] );
                Serial1.print("\t");
                Serial1.print( value[2] );
                Serial1.println();
            }
            if (whatToDebug == 1)
            {
                int gyroinfo[3];
                int *value(gyroinfo);
                inu.getMegData(value);
                Serial1.print( value[0] );
                Serial1.print("\t");
                Serial1.print( value[1] );
                Serial1.print("\t");
                Serial1.print( value[2] );
                Serial1.println();
            }

            if (whatToDebug == 2)
            {
                int gyroinfo[3];
                int *value(gyroinfo);
                inu.getaccData(value);
                Serial1.print( value[0] );
                Serial1.print("\t");
                Serial1.print( value[1] );
                Serial1.print("\t");
                Serial1.print( value[2] );
                Serial1.println();
            }
        }
    }

    correctOffsetAndApplyMotorValues();
}


