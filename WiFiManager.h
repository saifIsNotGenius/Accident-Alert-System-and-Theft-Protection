#line 1 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\WiFiManager.h"
#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

class WiFiManager {
public:
  WiFiManager(const char* ssid, const char* password);

  void begin();
  bool isConnected();
  void reconnect();

  bool sendToTelegram(const String& botToken,
                      const String& chatId,
                      const String& message);
  String getEmergencyNumber();
  int getWiFiQuality();

private:
  const char* ssid;
  const char* password;
  WiFiClientSecure client;
};
