#line 1 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\GyroManager.cpp"
#include "GyroManager.h"

bool isI2CBusActive() {
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0)
      return true;
  }
  return false;
}

GyroManager::GyroManager() {}

void GyroManager::begin() {
  Wire.begin();  // default SDA=21, SCL=22 on ESP32
  mySensor.setWire(&Wire);
  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();
  delay(100);
}

void GyroManager::update() {
  mySensor.accelUpdate();
  mySensor.gyroUpdate();
  mySensor.magUpdate();
}

float GyroManager::getAccX() {
  return mySensor.accelX();
}
float GyroManager::getAccY() {
  return mySensor.accelY();
}
float GyroManager::getAccZ() {
  return mySensor.accelZ();
}

float GyroManager::getGyroX() {
  return mySensor.gyroX();
}
float GyroManager::getGyroY() {
  return mySensor.gyroY();
}
float GyroManager::getGyroZ() {
  return mySensor.gyroZ();
}

// simple roll, pitch, yaw calculation from accelerometer
float GyroManager::getRoll() {
  return atan2(getAccY(), getAccZ()) * 180.0 / PI;
}

float GyroManager::getPitch() {
  return atan2(-getAccX(), sqrt(getAccY() * getAccY() + getAccZ() * getAccZ())) * 180.0 / PI;
}

float GyroManager::getYaw() {
  // Yaw needs magnetometer and calibration
  float mx = mySensor.magX();
  float my = mySensor.magY();
  return atan2(my, mx) * 180.0 / PI;
}
