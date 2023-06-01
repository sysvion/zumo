#include "inertial.h"
#include <Zumo32U4.h>
#include <Arduino.h>
#include <Wire.h>

inertial::inertial() {}

void inertial::setup()
{
  Wire.begin();

  {
  if (!sensors.init())
    // check if wire is started
    DebugRegTest(String("LSM303D"),  0b0011101, 0x0F, 0x49);
    DebugRegTest(String("L3GD20H"),  0b1101011, 0x0F, 0xD7);
    DebugRegTest(String("LSM6DS33"), 0b1101011, 0x0F, 0x69);
    DebugRegTest(String("L3GD20H"),  0b0011110, 0x0F, 0x3D);
    // make sure wire is started.

    ledRed(1);
    while(1)
    {
      Serial.println("failed to initulize inertial");
      for (int i=1; i > 10; i++ ){
        Serial.print(".");
        delay(300);
      }
      delay(500);
    }
  }
  calabrateGyro();


}

int8_t inertial::DebugRegTest(String identifierString, uint8_t addr, uint8_t reg, int16_t whoAmI) const
{
  Serial.println("debuging " + identifierString);
  Wire.beginTransmission(addr);
  Wire.write(reg);
  if (Wire.endTransmission() != 0)
  {
    Serial.println("failed endTransmission test");
    return 1; 
  }
 
  uint8_t byteCount = Wire.requestFrom(addr, (uint8_t)1);
  if (byteCount != 1)
  {
    Serial.println("failed byteCount test");
    return 2;
  }
  int out = Wire.read();
  Serial.println("got " + String(out) + " from wire.read" );
  
  if (Wire.read() == whoAmI) {
      Serial.println("failed whoami test");
      return 3;    
  }

  return 0;
}

// array is of size 3
// [0] roll left/right
// [1] pich front/back
// [2] yaw left/right
void inertial::getGyroPoss(int *array) {
  if (sensors.gyroDataReady()) {
    sensors.readGyro();

    array[0] = sensors.g.x - gyroOffset[0];
    array[1] = sensors.g.y - gyroOffset[1];
    array[2] = sensors.g.z - gyroOffset[2];
  }
}

void inertial::getMegData(int *array) {
      if (sensors.magDataReady()) {
    sensors.readMag();

    array[0] = sensors.m.x;
    array[1] = sensors.m.y;
    array[2] = sensors.m.z;
  }
}
void inertial::getaccData(int *array) {
  if (sensors.accDataReady()) {
    sensors.readAcc();

    array[0] = sensors.a.x;
    array[1] = sensors.a.y;
    array[2] = sensors.a.z;
  }
}
 

// source: example->zumo32u3->RotationResist->Turnsensor.h:turnSensorSetup()
void inertial::calabrateGyro() {
  int32_t totalx = 0;
  int32_t totaly = 0;
  int32_t totalz = 0;

  const int samplePoints = 1024;

  for (uint16_t i = 0; i < samplePoints; i++)
  {
    // Wait for new data to be available, then read it.
    while(!sensors.gyroDataReady()) {}
    sensors.readGyro();

    // Add the Z axis reading to the total.
    totalx += sensors.g.x;
    totaly += sensors.g.y;
    totalz += sensors.g.z;
  }
  gyroOffset[0] = totalx / samplePoints;
  gyroOffset[1] = totaly / samplePoints;
  gyroOffset[2] = totalz / samplePoints;
}

bool inertial::checkDownwardSpeed() {
  int gyroinfo[3];
  int *value(gyroinfo);
  getGyroPoss(value);  
  return value[1] > -300;
}

void inertial::print() {
  char report[120];
  sensors.read();
  snprintf_P(report, sizeof(report),
    PSTR("\t%6d\t%6d\t%6d\t%6d\t%6d\t%6d\t%6d\t%6d\t%6d\t"),
    sensors.a.x, sensors.a.y, sensors.a.z,
    sensors.m.x, sensors.m.y, sensors.m.z,
    sensors.g.x - gyroOffset[0], sensors.g.y - gyroOffset[1], sensors.g.z - gyroOffset[2]);

  Serial.println(report);
}

int inertial::check() {
  //  logic
}
