#line 1 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\StorageManager.cpp"
#include "StorageManager.h"

static File f;
const String EMERGENCY_CACHE_FILE = "/emergency_cache.txt";

void StorageManager::begin(size_t maxSize) {
  this->maxSize = maxSize;

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed");
    return;
  }

  f = LittleFS.open(filename, "a");
  if (!f) Serial.println("Failed to open telemetry.bin");
}

bool StorageManager::appendTelemetry(float lat, float lon, float speed, unsigned long timestamp, bool crash) {
  if (!f) return false;
  if (!lat || !lon || !timestamp) return false;

  Telemetry t = { lat, lon, speed, timestamp, crash };

  ensureCapacity();

  // Write binary data
  f.write((uint8_t *)&t, sizeof(Telemetry));
  f.flush();
  return true;
}

void StorageManager::ensureCapacity() {
  size_t fileSize = f.size();

  if (fileSize + sizeof(Telemetry) > maxSize) {
    // Clear entire file
    f.close();
    LittleFS.remove(filename);
    f = LittleFS.open(filename, "a");
  }
}

void StorageManager::del() {
  f.close();
  LittleFS.remove(filename);
  f = LittleFS.open(filename, "a");
}

bool StorageManager::readTelemetry(uint32_t index, Telemetry &t) {
  File r = LittleFS.open(filename, "r");
  if (!r) return false;

  size_t pos = index * sizeof(Telemetry);
  if (pos >= r.size()) {
    r.close();
    return false;  // out of range
  }

  r.seek(pos);
  r.read((uint8_t *)&t, sizeof(Telemetry));
  r.close();
  return true;
}

// Save emergency number to LittleFS
bool StorageManager::cacheEmergencyNumber(const String &number) {
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed");
    return false;
  }

  File f = LittleFS.open(EMERGENCY_CACHE_FILE, "w");
  if (!f) {
    Serial.println("Failed to open emergency cache file");
    return false;
  }

  f.print(number);
  f.close();
  return true;
}

// Load cached emergency number
String StorageManager::getCachedEmergencyNumber() {
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed");
    return "01700000000";  // default fallback
  }

  File f = LittleFS.open(EMERGENCY_CACHE_FILE, "r");
  if (!f) {
    Serial.println("No cached emergency number found");
    return "01700000000";  // default fallback
  }

  String number = f.readString();
  f.close();

  number.trim();                                   // remove any trailing newline
  if (number.length() == 0) return "01700000000";  // fallback

  return number;
}
