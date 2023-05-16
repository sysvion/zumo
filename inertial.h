#pragma one
#include <Zumo32U4.h>
class inertial {
  private:
    Zumo32U4IMU sensors;
    int gyroOffset[2];


  public:
    inertial();
    inertial ( const inertial & ); 
    class position;
    void setup();
    void calabrateGyro();
    uint16_t * getGyroPoss();
    //void inertial::getAll(uint16_t[2][2] a);
    void print();

    int check();
    
    
};
