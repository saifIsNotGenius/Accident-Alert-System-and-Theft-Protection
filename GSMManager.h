#line 1 "C:\\Users\\LENOVO\\Documents\\Arduino\\main\\GSMManager.h"
#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>

class GSMManager {
public:
  GSMManager(uint8_t serialNum);                                        // constructor: specify UART pins
  void begin(int RX, int TX);                                           // initialize SIM800L
  void sendSMS(const String &number, const String &message);            // send SMS
  void makeCall(const String &number, unsigned long duration = 30000);  // call for duration
  void query(const String &cmd);

  HardwareSerial sim800;
};