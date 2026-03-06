#line 1 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\CrashDetector.h"
#pragma once
#include <Arduino.h>
#include "GPSManager.h"
#include "GyroManager.h"
#include <math.h>

class CrashDetector {
public:
  CrashDetector(GPSManager& gpsMgr, GyroManager& gyroMgr);

  void update();         // call every loop
  bool crashDetected();  // returns true if crash detected

private:
  GPSManager& gps;
  GyroManager& gyro;

  float prevSpeed;         // previous GPS speed in m/s
  unsigned long prevTime;  // timestamp of previous GPS sample

  // thresholds
  const float ACC_THRESHOLD = 5;           // m/s^2 (~3g)
  const float GYRO_THRESHOLD = 300.0;      // deg/s
  const float SPEED_DROP_THRESHOLD = 0.0;  // m/s (~54 km/h)
  const unsigned long TIME_WINDOW = 3000;  // 3 seconds in ms
  bool crashFlag;
};