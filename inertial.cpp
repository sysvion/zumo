#include "inertial.h"
#include <Zumo32U4.h>
#include <Arduino.h>
#include <Array.h>

inertial::inertial() {

}

class inertial::position {
  public:
  uint16_t x = NULL;
  uint16_t y = NULL;
  uint16_t z = NULL;
  position(uint16_t xi, uint16_t yi, uint16_t zi)
  :x(xi),y(yi),z(zi) {}
};

void inertial::setup(){
  if (!sensors.init())
  {
    // Failed to detect the compass.
    ledRed(1);
    while(1)
    {
      Serial.println(F("Failed to initialize IMU sensors."));
      delay(100);
    }
  }
  sensors.enableDefault();
  
  calabrateGyro();
}


uint16_t *inertial::getGyroPoss() {
  sensors.readGyro();
  uint16_t res[3] = {sensors.g.x - gyroOffset[0] , sensors.g.y - gyroOffset[1] , sensors.g.z - gyroOffset[2] };
    //Serial.print(res[0]);
    //Serial.print("\t");
    //Serial.print(res[1]);
    //Serial.print("\t");
    Serial.println(res[2]); // left right
  return res;
  
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

/*class inertial::poss {
public:
  uint16_t x;
  uint16_t y;
  uint16_t z;

  poss(uint16_t xi, uint16_t yi, uint16_t zi)
  :x(xi),
  y(yi),
  z(zi) {}
};

class inertial::all {
  poss a;
  poss m;
  poss g;

  all(poss ai, poss mi, poss gi)
    :a(ai),
    m(mi),
    g(gi) {}
};*/

//void inertial::getAll(uint16_t[2][2]&) {
 
//}

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
