#line 1 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\GyroManager.h"
#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <MPU9250_asukiaaa.h>

bool isI2CBusActive();

class GyroManager {
public:
  GyroManager();  // constructor
  void begin();   // initialize I2C & MPU9250
  void update();  // read data from sensor
  float getAccX();
  float getAccY();
  float getAccZ();
  float getGyroX();
  float getGyroY();
  float getGyroZ();
  float getRoll();
  float getPitch();
  float getYaw();

private:
  MPU9250_asukiaaa mySensor;
};
