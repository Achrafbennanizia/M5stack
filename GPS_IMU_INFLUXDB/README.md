# GPS + IMU Road Condition Monitoring System

A real-time road condition monitoring system built for M5Stack Core ESP32 that integrates GPS positioning, IMU accelerometer data, and cloud-based data logging via InfluxDB.

## 🎯 Project Overview

This project monitors road conditions by combining GPS location data with IMU (Inertial Measurement Unit) acceleration measurements. The system detects road bumps and rough surfaces in real-time while tracking the vehicle's position and transmitting all data to InfluxDB Cloud for analysis and visualization.

## ✨ Features

- **Real-time GPS Tracking**
  - Latitude/Longitude coordinates with 6 decimal precision
  - Satellite count and HDOP (accuracy indicator)
  - Date and time from GPS signal
- **IMU Accelerometer Monitoring**

  - 3-axis acceleration data (X, Y, Z) in milligrams (mg)
  - Magnitude calculation for bump detection
  - Configurable thresholds for road condition classification

- **Road Condition Detection**

  - 🟢 **Smooth ride**: Normal road conditions (< 1.2g)
  - 🟡 **Rough surface**: Moderate bumps (1.2g - 1.5g)
  - 🔴 **Bump detected**: Significant road irregularities (> 1.5g)

- **Distance Tracking**

  - Set start position with Button A
  - Calculate distance traveled from start point
  - Track elapsed time since start

- **Cloud Data Logging**

  - Automatic data transmission to InfluxDB Cloud every 5 seconds
  - WiFi connectivity with status monitoring
  - Includes GPS, IMU, and distance metrics

- **LCD Display**
  - Live data visualization in organized tables
  - WiFi and InfluxDB connection status
  - Color-coded road condition indicators

## 🛠️ Hardware Requirements

- **M5Stack Core ESP32**
- **GPS Module** connected via UART2
  - RX Pin: GPIO 16
  - TX Pin: GPIO 17
  - Baud Rate: 9600

## 📚 Software Dependencies

- [PlatformIO](https://platformio.org/)
- M5Unified Library (v0.1.16+)
- TinyGPSPlus Library (v1.0.3+)
- ESP8266 Influxdb Library (v3.13.2+)
- WiFi (ESP32 built-in)

## 🚀 Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/Achrafbennanizia/M5stack.git
cd M5stack/GPS_IMU_INFLUXDB
```

### 2. Configure WiFi and InfluxDB

Edit `src/main.cpp` and update the following configuration:

```cpp
// WiFi Configuration
#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"

// InfluxDB Configuration
#define INFLUXDB_URL "your-influxdb-url"
#define INFLUXDB_TOKEN "your-token"
#define INFLUXDB_ORG "your-org"
#define INFLUXDB_BUCKET "your-bucket"
#define MEASUREMENT_NAME "your-measurement-name"
```

### 3. Build and Upload

```bash
# Build the project
platformio run --environment m5stack-core-esp32

# Upload to device
platformio run --target upload --upload-port /dev/cu.usbserial-XXXXXX
```

### 4. Monitor Serial Output

```bash
platformio device monitor --port /dev/cu.usbserial-XXXXXX --baud 115200
```

## 📊 Data Structure

### InfluxDB Data Points

The system sends the following measurements to InfluxDB:

| Field                 | Type  | Description                            |
| --------------------- | ----- | -------------------------------------- |
| `latitude`            | float | GPS latitude in degrees                |
| `longitude`           | float | GPS longitude in degrees               |
| `satellites`          | int   | Number of satellites in view           |
| `hdop`                | float | Horizontal Dilution of Precision       |
| `speed`               | float | Speed in km/h                          |
| `acc_x`               | float | X-axis acceleration in g               |
| `acc_y`               | float | Y-axis acceleration in g               |
| `acc_z`               | float | Z-axis acceleration in g               |
| `acc_magnitude`       | float | Total acceleration magnitude in g      |
| `distance_from_start` | float | Distance from start position in meters |

**Tags:**

- `device`: "m5stack"
- `location`: "road_test"

## 🎮 Usage

1. **Power On**: The M5Stack will automatically connect to WiFi and InfluxDB
2. **Wait for GPS Fix**: Move to an outdoor location with clear sky view
3. **Set Start Position**: Press **Button A** when you want to begin tracking distance
4. **Monitor**: View real-time data on the LCD display
5. **Data Collection**: All metrics are automatically sent to InfluxDB Cloud

## 📱 Display Layout

```
┌─────────────────────────────────────────┐
│ Versuch - Road Condition Monitoring     │
│ WiFi: OK                                │
├─────────────────────────────────────────┤
│ Sats│HDOP│Latitude   │Longitude  │Date │
│  12 │1.2 │51.123456  │7.654321   │24/..│
├─────────────────────────────────────────┤
│ Latitude │Longitude │Dauer│Distanz     │
│ 51.123456│7.654321  │125s │1234.5m     │
├─────────────────────────────────────────┤
│ x-Accel │y-Accel │z-Accel              │
│ 15 mg   │-20 mg  │980 mg               │
├─────────────────────────────────────────┤
│ Road Condition: Smooth ride             │
│ BtnA: Set Start                         │
└─────────────────────────────────────────┘
```

## 🔧 Configuration Constants

You can customize the system behavior by modifying these constants in `main.cpp`:

```cpp
const uint32_t DISPLAY_UPDATE_INTERVAL = 1000;  // Display refresh rate (ms)
const uint32_t INFLUX_SEND_INTERVAL = 5000;     // Data upload interval (ms)
const float BUMP_THRESHOLD = 1.5f;              // Bump detection threshold (g)
const float ROUGH_THRESHOLD = 1.2f;             // Rough surface threshold (g)
```

## 📡 GPS Module Setup

Ensure your GPS module is connected properly:

- **VCC** → 5V or 3.3V (check your module specs)
- **GND** → GND
- **TX** (GPS) → GPIO 16 (M5Stack RX)
- **RX** (GPS) → GPIO 17 (M5Stack TX)

## 🐛 Troubleshooting

### GPS Not Getting Fix

- Ensure you're outdoors with clear sky view
- Wait 1-2 minutes for cold start
- Check GPS module wiring
- Verify baud rate (9600) matches your module

### WiFi Connection Failed

- Verify SSID and password are correct
- Check WiFi signal strength
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)

### InfluxDB Connection Error

- Verify token, URL, org, and bucket are correct
- Check internet connectivity
- Ensure InfluxDB Cloud account is active

### No Data on Display

- Check serial monitor for error messages
- Verify all libraries are installed correctly
- Try rebuilding the project: `pio run -t clean && pio run`

## 📈 Use Cases

- **Road Quality Assessment**: Collect data for municipal road maintenance planning
- **Vehicle Monitoring**: Track driving conditions and route analysis
- **Research Projects**: Gather real-world acceleration and GPS data
- **Smart City Applications**: Contribute to urban infrastructure monitoring

## 🤝 Contributing

Contributions are welcome! Feel free to submit issues or pull requests.

## 📄 License

This project is open source and available under the MIT License.

## 👥 Authors

- **Group**: Ahmed, Achraf, Bouker
- **Course**: HSOS IoT Project

## 🙏 Acknowledgments

- M5Stack for the excellent hardware platform
- TinyGPS++ library by Mikal Hart
- InfluxDB Arduino client by Tobias Schürg
- M5Unified library team

---

**Note**: This project was developed as part of an IoT course at Hochschule Osnabrück (HSOS).
