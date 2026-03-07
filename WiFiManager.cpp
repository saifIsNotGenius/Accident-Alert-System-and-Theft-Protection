#line 1 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\WiFiManager.cpp"
#include "WiFiManager.h"

WiFiManager::WiFiManager(const char* ssid, const char* password)
  : ssid(ssid), password(password) {}

void WiFiManager::begin() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.println(WiFi.localIP());
    client.setInsecure();  // skip SSL cert validation
  } else {
    Serial.println("\nWiFi failed");
  }
}

bool WiFiManager::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::reconnect() {
  if (!isConnected()) {
    WiFi.disconnect();
    WiFi.begin(ssid, password);
  }
}

bool WiFiManager::sendToTelegram(const String& botToken,
                                 const String& chatId,
                                 const String& message) {
  if (!isConnected()) return false;

  const char* host = "api.telegram.org";
  String url = "/bot" + botToken + "/sendMessage?chat_id=" + chatId + "&text=" + message;

  if (!client.connect(host, 443)) {
    Serial.println("Telegram connection failed");
    return false;
  }

  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  // Optional: read response
  while (client.connected() || client.available()) {
    if (client.available()) {
      client.read();
    }
  }

  client.stop();
  return true;
}

String WiFiManager::getEmergencyNumber() {
  if (!isConnected()) return "";

  client.setInsecure();  // skip certificate verification
  const char* host = "XXXXXXXXXXXXXXXX";
  const int httpsPort = 443;
  String url = "/emergency.json";  // Firebase path

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed!");
    return "";
  }

  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {  // 5s timeout
      client.stop();
      Serial.println("Timeout");
      return "";
    }
  }

  // Read response
  String payload = "";
  bool isBody = false;
  while (client.available()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {  // end of headers
      isBody = true;
      continue;
    }
    if (isBody) {
      payload += line;
    }
  }
  client.stop();

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

int WiFiManager::getWiFiQuality() {
  long rssi = WiFi.RSSI();
  if (rssi <= -100) return 0;
  if (rssi >= -50) return 100;
  return 2 * (rssi + 100);
}