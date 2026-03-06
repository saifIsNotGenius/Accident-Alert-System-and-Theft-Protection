#include <Arduino.h>
#include "GPSManager.h"
#include "GSMManager.h"
#include "GyroManager.h"
#include "WiFiManager.h"
#include "StorageManager.h"
#include "CrashDetector.h"
#include <HTTPClient.h>

#define WIFI_SSID "OnePlus"
#define WIFI_PASS "11223344"

#define BOT_TOKEN "8521652129:AAHzpePk5OuIEBMSBQhPPCbw30oK3hao1EE"
#define CHAT_ID "5103907880"

const String dbUrl = "https://accialertsystem-default-rtdb.asia-southeast1.firebasedatabase.app";

#define SWPIN 25

GPSManager gps(1);
WiFiManager wifi(WIFI_SSID, WIFI_PASS);
GSMManager gsm(2);
GyroManager gyro;
StorageManager storage;
CrashDetector crash(gps, gyro);

Telemetry t;
uint32_t lastSentIndex = 0;
unsigned long lastPush = 0;
unsigned long lastAppend = 0;

String emergency;

bool sendTelemetry(const Telemetry& t);
void pushTelemetry();
String getEmergencyNumber();

char cmd;
String whereNowNumber = "";

long cooldownForLeftBehind = 0;

#line 43 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\main.ino"
void setup();
#line 62 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\main.ino"
void loop();
#line 43 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\main.ino"
void setup() {
  pinMode(SWPIN, INPUT_PULLUP);
  Serial.begin(115200);
  // gps.begin(16, 17);  // RX, TX
  wifi.begin();
  gsm.begin(16, 17);  // RX, TX
  gyro.begin();
  storage.begin(1024 * 50);  // 50 KB
  delay(1000);
  if (wifi.isConnected()) {
    emergency = wifi.getEmergencyNumber();
    storage.cacheEmergencyNumber(emergency);
  } else {
    emergency = storage.getCachedEmergencyNumber();
  }
  Serial.println("Emergency Number: " + emergency);
  Serial.println("Starting...");
}

void loop() {
  Serial.printf("Lat = %d\n", gps.getLat());
  Serial.printf("X = %.3f, Y = %.3f, Z = %.3f\n", gyro.getGyroX(), gyro.getGyroY(), gyro.getGyroZ());
  Serial.printf("AccTotal = %.6f\n", sqrt(pow(gyro.getAccX(), 2) + pow(gyro.getAccY(), 2) + pow(gyro.getAccZ(), 2)));
  Serial.printf("GyroTotal = %.6f\n", sqrt(pow(gyro.getGyroX(), 2) + pow(gyro.getGyroY(), 2) + pow(gyro.getGyroZ(), 2)));

  gps.update();
  t.lat = gps.getLat();
  t.lon = gps.getLon();
  t.speed = gps.getSpeed();
  t.timestamp = gps.getTimestamp();
  t.crash = false;
  crash.update();
  if (crash.crashDetected()) {
    t.crash = true;
    Serial.println("Crash detected!");
  }

  cmd = Serial.read();
  if (cmd == 'm' || digitalRead(SWPIN) == LOW || t.crash) {
    Serial.println("SMS Sending...");
    char msg[200];
    snprintf(msg, sizeof(msg),
             "I got an accident! Save me! My location: https://www.google.com/maps/search/?api=1&query=%.6f,%.6f", t.lat, t.lon);
    Serial.println(msg);
    if (wifi.isConnected()) {
      wifi.sendToTelegram(BOT_TOKEN, CHAT_ID, msg);
    }
    gsm.sendSMS(emergency, msg);

    delay(1000);
  }

  if (wifi.isConnected() && wifi.getWiFiQuality() < 50 && millis() - cooldownForLeftBehind > 30 * 1000) {
    Serial.println("Theft MSG Sending...");
    String msg = "You got a bit too far from your vehicle.";
    if (wifi.isConnected()) {
      wifi.sendToTelegram(BOT_TOKEN, CHAT_ID, msg);
    }
    cooldownForLeftBehind = millis();  // intially zero
  }

  if (gsm.sim800.available()) {
    String header = gsm.sim800.readStringUntil('\n');
    header.trim();

    if (header.startsWith("+CMT:")) {
      int q1 = header.indexOf('"');
      int q2 = header.indexOf('"', q1 + 1);
      String sender = header.substring(q1 + 1, q2);

      String message = gsm.sim800.readStringUntil('\n');
      message.trim();

      Serial.println("SMS From: " + sender);
      Serial.println("Text: " + message);

      // Reply
      if (message == "wherenow") {
        char msg[200];
        snprintf(msg, sizeof(msg),
                 "My current location: https://www.google.com/maps/search/?api=1&query=%.6f,%.6f", t.lat, t.lon);
        Serial.println(msg);
        gsm.sim800.print("AT+CMGS=\"");
        gsm.sim800.print(sender);
        gsm.sim800.println("\"");
        delay(300);
        gsm.sim800.print(msg);
        gsm.sim800.write(26);
        delay(3000);
      }
    }
  }

  delay(100);  // adjust sampling rate

  if ((t.lat != 0 && t.lon != 0 && t.timestamp != 0) && millis() - lastAppend >= 5000) {
    storage.appendTelemetry(t.lat, t.lon, t.speed, t.timestamp, t.crash);
    lastAppend = millis();
  }

  if (wifi.isConnected() && millis() - lastPush >= 10000) {
    pushTelemetry();
    lastPush = millis();
  }
  // while (gsm.sim800.available()) {
  //   Serial.println(gsm.sim800.readStringUntil('\n'));
  // }
}

bool sendTelemetry(const Telemetry& t) {
  if (wifi.isConnected()) {
    Serial.println("WiFi not connected!");
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();  // Accept all SSL certs

  HTTPClient https;

  String url = dbUrl + "/telemetry/" + String(t.timestamp) + ".json";
  Serial.println("URL: " + url);

  if (!https.begin(client, url)) {
    Serial.println("HTTPS begin failed");
    return false;
  }

  https.addHeader("Content-Type", "application/json");

  String json = "{";
  json += "\"lat\":" + String(t.lat, 6) + ",";
  json += "\"lon\":" + String(t.lon, 6) + ",";
  json += "\"speed\":" + String(t.speed, 2) + ",";
  json += "\"timestamp\":" + String(t.timestamp) + ",";
  json += "\"crash\":" + String(t.crash ? "true" : "false");
  json += "}";

  Serial.println("JSON: " + json);

  int httpCode = https.PUT(json);
  Serial.print("HTTP Code: ");
  Serial.println(httpCode);

  https.end();

  return (httpCode > 0 && httpCode < 300);
}

void pushTelemetry() {
  // Keep reading and sending until no more new records
  while (storage.readTelemetry(lastSentIndex, t)) {
    // Skip invalid telemetry
    if (!(t.lat && t.lon && t.speed)) {
      Serial.println("Skipping invalid telemetry");
      lastSentIndex++;
      continue;
    }

    bool success = sendTelemetry(t);
    if (success) {
      Serial.println("Telemetry uploaded successfully");
      lastSentIndex++;  // Only increment if upload succeeded
    } else {
      Serial.println("Upload failed, will retry next cycle");
      break;  // Stop on first failure; retry next cycle
    }
  }

  if (!storage.readTelemetry(lastSentIndex, t)) {
    storage.del();
    lastSentIndex = 0;
    Serial.println("Deleted!");
  }
}

String getEmergencyNumber() {
  if (!wifi.isConnected()) return "";

  HTTPClient http;
  String url = dbUrl + "/emergency.json";

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("HTTP GET failed, code: %d\n", httpCode);
    http.end();
    return "";
  }

  String payload = http.getString();
  http.end();

  payload.trim();

  // Remove quotes if Firebase returned a string like "01700000000"
  if (payload.startsWith("\"") && payload.endsWith("\"")) {
    payload = payload.substring(1, payload.length() - 1);
  }

  if (payload.length() == 0) {
    Serial.println("Empty payload");
    return "";
  }

  return payload;
}
