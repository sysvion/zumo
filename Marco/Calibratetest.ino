
#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;

const uint16_t maxSpeed = 400;
uint16_t Speed = maxSpeed;
bool useEmitters = true;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];


void setup() {
  buzzer.play(">a32>>c32");
  lineSensors.initFiveSensors();
}

int CalibrateSensors_Green() {
  if (buttonA.getSingleDebouncedPress()) {
    lineSensors.calibrate();
    lineSensors.readCalibrated(lineSensorValues);
  }
}

void loop() {

  // Read the line sensors.
  lineSensors.read(lineSensorValues, useEmitters ? QTR_EMITTERS_ON : QTR_EMITTERS_OFF);

  CalibrateSensors_Green();
  int16_t Calibrate = lineSensors.readLine(lineSensorValues);

  char buffer[80];
  sprintf(buffer,"%4d %4d %4d %4d\n", lineSensorValues[0],lineSensorValues[2], lineSensorValues[2],lineSensorValues[2], lineSensorValues[4], Calibrate);
  Serial.print(buffer);


  if (lineSensorValues[2] == 0 ){ // White
    motors.setSpeeds(100,100);
  } else if (lineSensorValues[2] <= 1000){ // Black 20
    motors.setSpeeds(300,300);
    //Serial.println("Black");
  } else {
    motors.setSpeeds(0,0);
  }
}

