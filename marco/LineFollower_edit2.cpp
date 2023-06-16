/* This example uses the line sensors on the Zumo 32U4 to follow
a black line on a white background, using a PID-based algorithm.
It works decently on courses with smooth, 6" radius curves and
has been tested with Zumos using 75:1 HP motors.*/

#include <Wire.h>
#include <Zumo32U4.h>

// This is the maximum speed the motors will be allowed to turn.
const uint16_t MAX_SPEED = 300;
uint16_t speed = 200;
int count;
int colors = 0;
bool startMotor = false;

Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;

int16_t lastError = 0;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];  // creates an array with length 5
unsigned int lineSensorGreen[NUM_SENSORS] = {5000,5000,5000,5000,5000}; // creates an array for each color and put the value standard value te a non used value.
unsigned int lineSensorGray[NUM_SENSORS] = {5000,5000,5000,5000,5000};
unsigned int lineSensorBrown[NUM_SENSORS] = {5000,5000,5000,5000,5000};

int CalibrateSensors() { // Calibarates the lowest and highest gray value.
  lineSensors.calibrate();
  buzzer.play(">a32");
}

void CalibrateGreen() { // Copies the main array to a new array.
  if (buttonC.getSingleDebouncedPress()) {
    buzzer.play(">a32");
    colors = 1;
    for (int i; i<NUM_SENSORS; i++)
      lineSensorGreen[i] = lineSensorValues[i];
  }
}
void CalibrateGray() { // Copies the main array to a new array.
  if (buttonC.getSingleDebouncedPress()) {
    buzzer.play(">a32");
    colors = 2;
    for (int i; i<NUM_SENSORS; i++)
      lineSensorGray[i] = lineSensorValues[i];
  }
}
void CalibrateBrown() { // Copies the main array to a new array.
  if (buttonC.getSingleDebouncedPress()) {
    buzzer.play(">a32");
    colors = 10;
    startMotor = true;
    for (int i; i<NUM_SENSORS; i++)
      lineSensorBrown[i] = lineSensorValues[i];
  }
}

void setup() { // Before the ZUMO starts with the loop.
  lineSensors.initFiveSensors();
  buzzer.play(">a32>>c32");

  buttonB.waitForButton();
  CalibrateSensors();
  buttonB.waitForButton();
  CalibrateSensors();
}

int readLine(unsigned int *sensor_values, unsigned char readMode = 1, unsigned char white_line = 0) { // Function for reading the line.

  int lastValue;

  unsigned char i, on_line = 0;
  unsigned long avg;  // this is for the weighted total, which is long before division
  unsigned int sum;   // this is for the denominator which is <= 64000

  lineSensors.readCalibrated(sensor_values, readMode);

  avg = 0;
  sum = 0;

  for (i = 0; i < NUM_SENSORS; i++) {
    int value = sensor_values[i];
    if (white_line)
      value = 1000 - value;

    // keep track of whether we see the line at all
    if (value > 4) { //200 default
      on_line = 1;
    }

    // only average in values that are above a noise threshold
    if (value > 50) {
      avg += (long)(value) * (i * 1000);
      sum += value;
    }
  }

  if (!on_line) {
    // If it last read to the left of center, return 0.
    if (lastValue < (NUM_SENSORS - 1) * 1000 / 2)
      return 0;
    // If it last read to the right of center, return the max.
    else
      return (NUM_SENSORS - 1) * 1000;
  }

  lastValue = avg / sum;

  return lastValue;
}

void loop() {

  int16_t position = readLine(lineSensorValues);

  // "error" is how far we are away from the center of the line, which corresponds to position 2000.
  int16_t error = position - 2000;

  // Get motor speed difference using proportional and derivative PID terms (the integral term is generally not very
  // useful for line following).  Here we are using a proportional constant of 1/4 and a derivative constant of 6.
  int16_t speedDifference = error / 1 + 3 * (error - lastError);

  lastError = error;

  // Get individual motor speeds. The sign of speedDifference
  // determines if the robot turns left or right.
  int16_t leftSpeed = speed + speedDifference;
  int16_t rightSpeed = speed - speedDifference;

  // Constrain motor speeds to be between 0 and MAX_SPEED.
  // One motor will always be turning at MAX_SPEED, and the other
  // will be at MAX_SPEED-|speedDifference| if that is positive,
  // else it will be stationary.
  leftSpeed = constrain(leftSpeed, 0, MAX_SPEED);
  rightSpeed = constrain(rightSpeed, 0, MAX_SPEED);

  if (colors == 0) { // Makes sure each color will always be calibrated in order.
    CalibrateGreen();
  } else if (colors == 1) {
    CalibrateGray();
  } else if (colors == 2) {
    CalibrateBrown();
  }

  if (startMotor) { // Only lets the motor move when this bool is true.
    if ((lineSensorValues[2] > lineSensorGreen[2]-30) && (lineSensorValues[2] < lineSensorGreen[2]+30)) { // If the color is green
      leftSpeed = 100;
      rightSpeed = 100;
    }

    if ((lineSensorValues[0] > lineSensorGray[0]-30) && (lineSensorValues[0] < lineSensorGray[0]+30) || 
        (lineSensorValues[4] > lineSensorGray[4]-30) && (lineSensorValues[4] < lineSensorGray[4]+30)) { // If the color is gray.
      //leftSpeed = -200;
      //rightSpeed = 200;
    }

    if ((lineSensorValues[2] > lineSensorBrown[2]-30) && (lineSensorValues[2] < lineSensorBrown[2]+30)) { // If the color is brown.
        // Ga naar modus: blok duwen.
    }
    
    motors.setSpeeds(leftSpeed, rightSpeed);


  }
  
  char buffer[80];
  //sprintf(buffer, "%5d %5d %5d %5d %5d %5d %5d\n", lineSensorValues[0], lineSensorValues[1], lineSensorValues[2], lineSensorValues[3], lineSensorValues[4], error, speedDifference);
  sprintf(buffer, "%5d %5d \n", lineSensorBrown[2], lineSensorValues[2]);
  count++;
  if (count == 25) {
    Serial.print(buffer);
    count = 0;
  }
}