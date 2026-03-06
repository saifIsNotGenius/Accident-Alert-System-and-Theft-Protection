#line 1 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\CrashDetector.cpp"
#include "CrashDetector.h"

CrashDetector::CrashDetector(GPSManager& gpsMgr, GyroManager& gyroMgr)
  : gps(gpsMgr), gyro(gyroMgr), prevSpeed(0), prevTime(0), crashFlag(false) {}

void CrashDetector::update() {
  gps.update();
  gyro.update();

  unsigned long now = millis();

  // current GPS speed (m/s)
  float speed = gps.getSpeed();

  // accelerometer magnitude (m/s^2)
  float accTotal = sqrt(
    gyro.getAccX() * gyro.getAccX() + gyro.getAccY() * gyro.getAccY() + gyro.getAccZ() * gyro.getAccZ());

  // gyroscope magnitude (deg/s)
  float gyroTotal = sqrt(
    gyro.getGyroX() * gyro.getGyroX() + gyro.getGyroY() * gyro.getGyroY() + gyro.getGyroZ() * gyro.getGyroZ());

  // time difference (ms)
  unsigned long dt = (prevTime > 0) ? now - prevTime : 0;

  // speed drop (m/s)
  float speedDrop = prevSpeed - speed;

  // Detect crash
  if (dt > 0 && dt <= TIME_WINDOW && accTotal > ACC_THRESHOLD && gyroTotal > GYRO_THRESHOLD && speedDrop >= SPEED_DROP_THRESHOLD) {
    crashFlag = true;
  }

  prevSpeed = speed;
  prevTime = now;
}

bool CrashDetector::crashDetected() {
  if (crashFlag) {
    crashFlag = false;
    return true;
  }
  return false;
}