#line 1 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\GPSManager.h"
#pragma once
#include <Arduino.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <TimeLib.h>

class GPSManager {
public:
  GPSManager(uint8_t serialNum);

  void begin(int RX, int TX);
  void update();  // read data from sensor

  float getLat();
  float getLon();
  float getSpeed();  // mps
  float getAltitude();
  unsigned long getTimestamp();

private:
  TinyGPSPlus gps;
  HardwareSerial gpsSerial;
};