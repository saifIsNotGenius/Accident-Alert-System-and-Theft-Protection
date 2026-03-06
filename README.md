```markdown
# AcciAlert System 🚗⚠️

AcciAlert System is an ESP32-based smart vehicle safety device that detects accidents and automatically sends the vehicle location to emergency contacts. It also allows remote location queries via SMS and continuously uploads telemetry data to the cloud.

The system integrates **GPS, GSM, WiFi, and a gyroscope/accelerometer sensor** to monitor vehicle movement and detect crashes in real time.

---

## Features

### 🚨 Crash Detection
- Uses accelerometer and gyroscope data to detect possible vehicle accidents.
- Automatically sends an emergency alert when a crash is detected.

### 📍 Live Location Sharing
- Sends a Google Maps location link via:
  - SMS
  - Telegram bot

### 📡 Remote Location Request
- Send an SMS with the command:

```

wherenow

```

The device replies with its current location.

### 📶 Vehicle Theft / Left-Behind Alert
- Detects when the phone moves far away from the vehicle (based on WiFi signal quality).
- Sends a warning message.

### ☁️ Cloud Telemetry
- Periodically uploads telemetry data:
  - Latitude
  - Longitude
  - Speed
  - Timestamp
  - Crash status

- Data is stored in Firebase Realtime Database.

### 💾 Offline Data Storage
- Telemetry is cached locally if internet is unavailable.
- Automatically uploaded once connection is restored.

### 🔘 Manual SOS Button
Pressing the hardware switch sends an emergency alert immediately.

---

## Hardware Requirements

- ESP32
- GPS Module (e.g. NEO-6M)
- GSM Module (SIM800L)
- Gyroscope / Accelerometer (e.g. MPU6050)
- Push Button (SOS switch)
- Power supply / vehicle power

---

## System Architecture

```mermaid
graph TD

GPS[GPS Module] --> ESP[ESP32 Controller]
GYRO[Gyroscope / Accelerometer] --> ESP

ESP --> WIFI[WiFi]
ESP --> GSM[GSM Module]

WIFI --> FIREBASE[Firebase Database]
WIFI --> TELEGRAM[Telegram Bot]

GSM --> SMS[SMS Alerts]

---

### Modules

| Module | Purpose |
|------|------|
| `GPSManager` | Handles GPS data parsing and updates |
| `GSMManager` | Sends SMS and reads incoming messages |
| `GyroManager` | Reads accelerometer and gyroscope data |
| `WiFiManager` | WiFi connection, Telegram messaging |
| `StorageManager` | Local telemetry caching |
| `CrashDetector` | Crash detection logic |

---

## Configuration

Edit the following parameters in the code:

```cpp
#define WIFI_SSID "your_wifi"
#define WIFI_PASS "your_password"

#define BOT_TOKEN "telegram_bot_token"
#define CHAT_ID "telegram_chat_id"
````

Firebase database URL:

```cpp
const String dbUrl = "https://your-database-url.firebaseio.com";
```

Emergency phone number is retrieved from Firebase and cached locally.

---

## Telemetry Format

Data uploaded to Firebase:

```json
{
  "lat": 23.123456,
  "lon": 90.123456,
  "speed": 45.6,
  "timestamp": 1710000000,
  "crash": false
}
```

---

## Emergency Message Example

```
I got an accident! Save me!
My location:
https://www.google.com/maps/search/?api=1&query=23.123456,90.123456
```

---

## How It Works

1. Device continuously reads GPS and sensor data.
2. Crash detection algorithm analyzes acceleration and rotation.
3. If a crash is detected:

   * SMS is sent to emergency contact.
   * Telegram alert is sent.
4. Telemetry is stored locally and periodically uploaded to Firebase.
5. Users can request location anytime via SMS.

---

## Future Improvements

* Mobile app dashboard
* Real-time vehicle tracking
* Improved crash detection with machine learning
* Battery backup system
* OTA firmware updates

---

## License

MIT License

```