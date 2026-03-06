#line 1 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\GPSManager.cpp"
#include "GPSManager.h"

GPSManager::GPSManager(uint8_t serialNum)
  : gpsSerial(serialNum) {}

void GPSManager::begin(int RX, int TX) {
  gpsSerial.begin(9600, SERIAL_8N1, RX, TX);
}

void GPSManager::update() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }
}

float GPSManager::getLat() {
  return gps.location.isValid() ? gps.location.lat() : 0;
}

float GPSManager::getLon() {
  return gps.location.isValid() ? gps.location.lng() : 0;
}

float GPSManager::getSpeed() {
  return gps.speed.isValid() ? gps.speed.mps() : 0;
}

float GPSManager::getAltitude() {
  return gps.altitude.isValid() ? gps.altitude.meters() : 0;
}

unsigned long GPSManager::getTimestamp() {
  if (gps.date.isValid() && gps.time.isValid()) {
    tmElements_t tm;
    tm.Year = gps.date.year() - 1970;
    tm.Month = gps.date.month();
    tm.Day = gps.date.day();
    tm.Hour = gps.time.hour();
    tm.Minute = gps.time.minute();
    tm.Second = gps.time.second();
    return makeTime(tm);
  }
  return 0;
}
