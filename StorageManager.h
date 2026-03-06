#line 1 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\StorageManager.h"
#pragma once
#include <Arduino.h>
#include <LittleFS.h>

struct Telemetry {
  float lat;
  float lon;
  float speed;
  unsigned long timestamp;  // GPS epoch seconds
  bool crash;
};

class StorageManager {
public:
  void begin(size_t maxSize);
  bool appendTelemetry(float lat, float lon, float speed,
                       unsigned long timestamp, bool crash);
  bool readTelemetry(uint32_t index, Telemetry &t);
  void del();
  bool cacheEmergencyNumber(const String &number);
  String getCachedEmergencyNumber();

private:
  void ensureCapacity();
  String filename = "/telemetry.bin";
  size_t maxSize;  // bytes
};
