#line 1 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\GSMManager.cpp"
#include "GSMManager.h"

GSMManager::GSMManager(uint8_t serialNum)
  : sim800(serialNum) {}

void GSMManager::begin(int RX, int TX) {
  sim800.begin(9600, SERIAL_8N1, RX, TX);
  delay(1000);

  Serial.println("SIM800L initialized...");

  sim800.println("AT");
  delay(200);

  sim800.println("AT+CMGF=1");  // SMS text mode
  delay(200);

  sim800.println("AT+CNMI=2,2,0,0,0");  // Direct SMS to serial
  delay(200);

  while (sim800.available())
    Serial.print(sim800.readString());
}

void GSMManager::sendSMS(const String &number, const String &message) {
  Serial.println("Sending SMS...");

  sim800.println("AT+CMGF=1");  // text mode
  delay(500);

  sim800.print("AT+CMGS=\"");
  sim800.print(number);
  sim800.println("\"");
  delay(500);

  sim800.print(message);
  delay(500);

  sim800.write(26);  // CTRL+Z
  delay(1000);

  Serial.println("SMS sent.");
}

void GSMManager::makeCall(const String &number, unsigned long duration) {
  Serial.println("Making emergency call...");

  sim800.print("ATD");
  sim800.print(number);
  sim800.println(";");
  delay(duration);        // call duration in milliseconds
  sim800.println("ATH");  // hang up
  delay(1000);

  Serial.println("Call ended.");
}

void GSMManager::query(const String &cmd) {
  Serial.println(cmd);
  sim800.println(cmd);
  delay(30 * 1000);
  while (sim800.available()) Serial.print(sim800.readString());
}