#include <Wire.h>
#include <Zumo32U4.h>
#include <math.h>

Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
Zumo32U4ButtonC buttonC;

char inputChar;

double speed = 1;             //starting speed (no movement)
double speedRight;
double speedLeft;
const int MIN_SPEED = -400;
const int MAX_SPEED = 400;

double steerRight = 1;        //default steer value. Value is later on multiplied with so the default is 1
double steerLeft = 1;
double steerIntensity = 1.3;  //intensity of steering changes

bool isDriving = true;            //determines if the robot should move or pause

const int MIN_VOLUME = 6;
const int MAX_VOLUME = 13;
int volume = MIN_VOLUME;

int count = 0;

void setup()
{
  Serial.begin(9600);         //start serial connection with the Arduino serial
  Serial1.begin(9600);        //start serial connection with the XCTU application (Xbee serial)

  // why not !Serial1
  while (!Serial/*isReady()*/) {}

  //prints into Serial1 how to use the control keys
  Serial1.println("\n\nZUMO MANUAL MODE\n  WASD to move\n  SPACE to stop/continue\n  Q to mote at max speed\n  E to reset rotational movement\n  R to reset movement\n -/+ keys to change volume\n");
}

//function to simplify/shorten the use of buzzer in the main loop
void sound(int frequency, int durationMilliseconds)
{
  buzzer.playFrequency(frequency, durationMilliseconds, volume);
}

void stopContinue()
{
  if (isDriving)
    {
      isDriving = false;
      sound(300, 80);
      delay(120);
      sound(280, 80);
      return;
    }
  isDriving = true;           //starts the robot
  sound(280, 80);
  delay(120);
  sound(300, 80);
}

void lowerVolume()
{
                                    // is this true?
  if (volume > MIN_VOLUME)          //checks if volume is already at maximum
  {
    volume -= 1;
    Serial1.println((String)"Volume: " + (volume-6));
    sound(460, 300);
  }
}

void increaseVolume()
{
  if (volume < MAX_VOLUME)          //checks if volume is already at minimum
  {
    volume += 1;
    Serial1.println((String)"Volume: " + (volume-6));
    sound(460, 300);
  }
}

void moveLeft()
{
    if (steerRight < 6)               //checks if right wheel steer is below max allowed value
    {
        return;
    }

    if  (steerLeft < 1)             //checks if left wheel steer is below default steer value
    {
        steerRight *= steerIntensity; //increase right wheel speed
    }
    else
    {
        steerLeft /= steerIntensity;  //reduce left wheel speed if condition not met
    }

    sound(400, 200);
}

void moveRight()
{
    sound(420, 200);
    if (steerLeft < 6)                //checks if left wheel steer is below max allowed value
    {
        return;
    }
    if (steerRight < 1)             //checks if rigt wheel steer is below default steer value
    {
        steerLeft *= steerIntensity;  //increase left wheel speed
    }
    else
    {
        steerRight /= steerIntensity; //decrease right wheel speed if condition not met
    }
}

void moveSlower()
{
  if (speed > -10)                  //checks if speed is above mimimum allowed value (NOTE: speed within functions isn't the actual motor speed.
  {                                 //The actual speeds for the motors are calculated at the end using a formula)
    speed -= 1;
    Serial1.println((String)"Speed: " + speed + " --");
    sound(400, 100);
    delay(140);
    sound(400, 100);
  }
}

void moveFaster()                   //does the opposite as moveSlower(), to move faster
{
  if (speed < 10)                   //checks if speed is below maximum allowed value
  {
    speed += 1;
    Serial1.println((String)"Speed: " + speed + " ++");
    sound(540, 80);
    delay(120);
    sound(540, 80);
  }
}

void moveMaxSpeed()
{
  speed = 8;                        //set speed to the max
  Serial1.println((String)"Speed: " + speed + " ++");
  sound(600, 80);
  delay(120);
  sound(600, 100);
}

void resetSpeed()
{
  speed = 1;
  //This stops the robot because opposite steer is subtracted from speed in the formula that determines the final speed for both motors (1-1=0)
  steerLeft = 1;
  steerRight = 1;                         

  //sets drive t true so robot can immediately start to drive again when input it given in case the robot was paused before reset was pressed
  isDriving = true;                     
  Serial1.println("RESET SPEED");

  //blinks the yellow led 3 times
  ledYellow(1);

  sound(280, 80);
  delay(50);
  ledYellow(0);
  delay(50);
  ledYellow(1);
  sound(280, 80);
  delay(100);
  ledYellow(0);
  sound(280, 80);
}

void resetRotationalMovement()
{ 
  //resets steer values but not the overal speed so the robots starts driving straight forward
  steerLeft = 1;
  steerRight = 1;
  Serial1.println("RESET ROTATION");
  ledYellow(1);                     //blinks the yellow led 3 times
  sound(280, 80);
  delay(50);
  ledYellow(0);
  delay(50);
  ledYellow(1);
  sound(300, 80);
  delay(100);
  ledYellow(0);
  sound(280, 80);
}

void rotateDeg(int deg)
{
  double rotateSpeed = speed * 40;            //set rotateSpeed to motor speed
  if (speed <= 2) rotateSpeed = 120;
  deg /= pow(rotateSpeed/600,1.7);            //robot will make a longer turn if moving slower to account for more resistance at lower speeds
    	                                        //1st num in pow: smaller = shorter turn. 2nd num in pow: higher = longer turn for lower speeds
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
  sound(200, 250);
  delay(250);
}

void loop()
{                                           //when button C is pressed, message how to use the control keys is printed into Serial1 again
  if (buttonC.isPressed()) {
      Serial1.println("\n\nZUMO MANUAL MODE\n  WASD to move\n  SPACE to stop/continue\n  Q to mote at max speed\n  E to reset rotational movement\n  R to reset movement\n -/+ keys to change volume\n");
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
      //volume -
      case '-':                     //if input char is "-", call lowerVolume() function. The same is done for every other key which call other functions
        lowerVolume();
        break;                      //break out of switch

      //volume +
      case '=':
        increaseVolume();
        break;

      //move left
      case 'a':
        moveLeft();
        break;

      //move right
      case 'd':
        moveRight();
        break;

      //move slower
      case 's':
        moveSlower();
        break;

      //move faster
      case 'w':
        moveFaster();
        break;

      //move at max speed
      case 'q':
        moveMaxSpeed();
        break;

      //reset speed
      case 'R':             //function resetSpeed() will also be called when capital R is pressed, in case robot needs to be stopped fast and CAPS LOCK is activated by accident
      case 'r':
        resetSpeed();
        break;

      //reset rotational movement
      case 'e':
        resetRotationalMovement();
        break;

      case 'A':             //rotate 90 deg left
        rotateDeg(90);
        break;

      case 'D':             //rotate 90 deg right
        rotateDeg(-90);
        break;

      case 'S':             //rotate 180 deg
        rotateDeg(-180);
        break;

      case '0':
        wait();
        break;

      default:              //"default:" is ran if none of other cases were activated. This is needed for the ' ' character (SPACEBAR) because this gives an error in a regular case.
        //stop/continue moving
        if (inputChar == ' ') stopContinue(); //if input is a space character, call stopContinue()
        break;
    }

    //these two formulas determine the final speeds for the left and right motor
    speedLeft  = (speed * steerLeft - steerRight) * 50;
    speedRight = (speed * steerRight - steerLeft) * 50 * 1.04;

    //these statements set the motor speeds to the minimum or maximum allowed value if these are above or below allowed vaues
    if (speedLeft > MAX_SPEED) speedLeft = MAX_SPEED;
    if (speedRight > MAX_SPEED) speedRight = MAX_SPEED;
    if (speedLeft < MIN_SPEED) speedLeft = MIN_SPEED;
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
      sound(200, 40);
      count = 0;
    }

    ledRed(1);    //blinks the red led 2 times
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
