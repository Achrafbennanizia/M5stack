#include <M5Unified.h>
#include <TinyGPSPlus.h>
#include <WiFi.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// ===== Configuration =====
#define WIFI_SSID "iPhone von Achraf"
#define WIFI_PASSWORD "Hello2020"
#define INFLUXDB_URL "https://eu-central-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "bCfPoZ_Ys5WvjAztMdlBUioe7e_8N9vAbCuxPEWFGVhJEEr-IfI9b8QxNxMBXRCWY7hKFvWpI4ONSmFis5zwUQ=="
#define INFLUXDB_ORG "hsos"
#define INFLUXDB_BUCKET "iot-bucket"
#define MEASUREMENT_NAME "group_Ahmed_Achraf_Bouker"
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define GPS_BAUD 9600

// ===== Constants =====
const uint32_t DISPLAY_UPDATE_INTERVAL = 1000;
const uint32_t INFLUX_SEND_INTERVAL = 5000;
const float BUMP_THRESHOLD = 1.5f;
const float ROUGH_THRESHOLD = 1.2f;

// ===== Global Objects =====
TinyGPSPlus gps;
HardwareSerial GPSSerial(2);
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensorData(MEASUREMENT_NAME);

// ===== State =====
unsigned long lastUpdate = 0;
unsigned long lastInfluxSend = 0;

struct StartPosition {
  double lat = 0.0;
  double lon = 0.0;
  unsigned long time = 0;
  bool isSet = false;
} startPosition;

// ===== Data Structures =====
struct IMUData {
  float ax, ay, az;
  float magnitude;
};

struct GPSData {
  double lat, lon;
  uint8_t satellites;
  float hdop;
  bool isValid;
};

// ===== Helper Functions =====
void drawSeparator(int y) {
  M5.Lcd.setCursor(5, y);
  M5.Lcd.print("--------------------------------------------");
}

IMUData readIMU() {
  IMUData data = {0};
  if (M5.Imu.isEnabled()) {
    M5.Imu.getAccel(&data.ax, &data.ay, &data.az);
    data.magnitude = sqrt(data.ax * data.ax + data.ay * data.ay + data.az * data.az);
  }
  return data;
}

GPSData getGPSData() {
  GPSData data = {0};
  data.isValid = gps.location.isValid();
  data.lat = data.isValid ? gps.location.lat() : 0.0;
  data.lon = data.isValid ? gps.location.lng() : 0.0;
  data.satellites = gps.satellites.isValid() ? gps.satellites.value() : 0;
  data.hdop = gps.hdop.isValid() ? gps.hdop.hdop() : 0.0f;
  return data;
}

const char* getRoadCondition(float magnitude) {
  if (magnitude > BUMP_THRESHOLD) return "!!! BUMP DETECTED !!!";
  if (magnitude > ROUGH_THRESHOLD) return "Rough surface";
  return "Smooth ride";
}

uint16_t getRoadConditionColor(float magnitude) {
  if (magnitude > BUMP_THRESHOLD) return RED;
  if (magnitude > ROUGH_THRESHOLD) return YELLOW;
  return GREEN;
}

void handleButtonPress() {
  if (M5.BtnA.wasPressed() && gps.location.isValid()) {
    startPosition.lat = gps.location.lat();
    startPosition.lon = gps.location.lng();
    startPosition.time = millis();
    startPosition.isSet = true;
    Serial.println(">>> START POSITION SET!");
  }
}

void processGPSData() {
  while (GPSSerial.available()) {
    gps.encode(GPSSerial.read());
  }
}

// ===== Initialization =====
void initializeHardware() {
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Imu.begin();
  
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== GPS + IMU + InfluxDB ===");
}

void initializeDisplay() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextFont(1);
  M5.Lcd.setCursor(20, 60);
  M5.Lcd.println("Road Condition Monitoring");
  M5.Lcd.setCursor(20, 80);
  M5.Lcd.println("Initializing...");
}

void initializeGPS() {
  GPSSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("GPS initialized");
}

bool connectWiFi() {
  M5.Lcd.setCursor(20, 100);
  M5.Lcd.print("WiFi: Connecting...");
  Serial.printf("Connecting to WiFi: %s\n", WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    M5.Lcd.print(".");
    attempts++;
  }
  
  bool connected = WiFi.status() == WL_CONNECTED;
  M5.Lcd.setCursor(20, 120);
  
  if (connected) {
    Serial.printf("\nWiFi Connected! IP: %s\n", WiFi.localIP().toString().c_str());
    M5.Lcd.setTextColor(GREEN, BLACK);
    M5.Lcd.println("WiFi: Connected!");
  } else {
    Serial.println("\nWiFi Failed!");
    M5.Lcd.setTextColor(RED, BLACK);
    M5.Lcd.println("WiFi: FAILED");
  }
  
  M5.Lcd.setTextColor(WHITE, BLACK);
  return connected;
}

void initializeInfluxDB() {
  sensorData.addTag("device", "m5stack");
  sensorData.addTag("location", "road_test");
  
  if (WiFi.status() != WL_CONNECTED) return;
  
  M5.Lcd.setCursor(20, 140);
  
  if (client.validateConnection()) {
    Serial.println("InfluxDB: Connected");
    M5.Lcd.setTextColor(GREEN, BLACK);
    M5.Lcd.println("InfluxDB: Connected");
  } else {
    Serial.printf("InfluxDB Error: %s\n", client.getLastErrorMessage().c_str());
    M5.Lcd.setTextColor(RED, BLACK);
    M5.Lcd.println("InfluxDB: Error");
  }
  
  M5.Lcd.setTextColor(WHITE, BLACK);
}

// ===== Display Functions =====
void displayHeader(int &y) {
  M5.Lcd.setCursor(20, y);
  M5.Lcd.print("Versuch - Road Condition Monitoring");
  y += 10;
  
  M5.Lcd.setCursor(5, y);
  M5.Lcd.setTextColor(WiFi.status() == WL_CONNECTED ? GREEN : RED, BLACK);
  M5.Lcd.print(WiFi.status() == WL_CONNECTED ? "WiFi: OK" : "WiFi: DISCONNECTED");
  M5.Lcd.setTextColor(WHITE, BLACK);
  y += 15;
}

void displayGPSTable(int &y, const GPSData &gpsData) {
  const int xSats = 5, xHdop = 35, xLat = 75, xLon = 155, xDate = 235;
  char buf[32];
  
  // Headers
  M5.Lcd.setCursor(xSats, y); M5.Lcd.print("Sats");
  M5.Lcd.setCursor(xHdop, y); M5.Lcd.print("HDOP");
  M5.Lcd.setCursor(xLat, y); M5.Lcd.print("Latitude");
  M5.Lcd.setCursor(xLon, y); M5.Lcd.print("Longitude");
  M5.Lcd.setCursor(xDate, y); M5.Lcd.print("Date");
  y += 10;
  
  M5.Lcd.setCursor(xLat, y); M5.Lcd.print("(deg)");
  M5.Lcd.setCursor(xLon, y); M5.Lcd.print("(deg)");
  M5.Lcd.setCursor(xDate, y); M5.Lcd.print("Time");
  y += 7;
  
  drawSeparator(y);
  y += 10;
  
  // Data
  M5.Lcd.setCursor(xSats, y);
  sprintf(buf, "%2d", gpsData.satellites);
  M5.Lcd.print(buf);
  
  M5.Lcd.setCursor(xHdop, y);
  sprintf(buf, gps.hdop.isValid() ? "%.2f" : "--", gpsData.hdop);
  M5.Lcd.print(buf);
  
  M5.Lcd.setCursor(xLat, y);
  sprintf(buf, "%.6f", gpsData.lat);
  M5.Lcd.print(buf);
  
  M5.Lcd.setCursor(xLon, y);
  sprintf(buf, "%.6f", gpsData.lon);
  M5.Lcd.print(buf);
  
  M5.Lcd.setCursor(xDate, y);
  if (gps.date.isValid()) {
    sprintf(buf, "%02d/%02d/%04d", gps.date.month(), gps.date.day(), gps.date.year());
  } else {
    sprintf(buf, "--/--/----");
  }
  M5.Lcd.print(buf);
  y += 10;
  
  M5.Lcd.setCursor(xDate, y);
  if (gps.time.isValid()) {
    sprintf(buf, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
  } else {
    sprintf(buf, "--:--:--");
  }
  M5.Lcd.print(buf);
  y += 15;
}

void displayPositionTable(int &y, const GPSData &gpsData) {
  const int xLat2 = 5, xLon2 = 95, xDauer = 185, xDist = 235;
  char buf[32];
  
  // Headers
  M5.Lcd.setCursor(xLat2, y); M5.Lcd.print("Latitude");
  M5.Lcd.setCursor(xLon2, y); M5.Lcd.print("Longitude");
  M5.Lcd.setCursor(xDauer, y); M5.Lcd.print("Dauer");
  M5.Lcd.setCursor(xDist, y); M5.Lcd.print("Distanz");
  y += 10;
  
  M5.Lcd.setCursor(xLat2, y); M5.Lcd.print("(deg)");
  M5.Lcd.setCursor(xLon2, y); M5.Lcd.print("(deg)");
  M5.Lcd.setCursor(xDauer, y); M5.Lcd.print("[s]");
  M5.Lcd.setCursor(xDist, y); M5.Lcd.print("[m]");
  y += 7;
  
  drawSeparator(y);
  y += 10;
  
  // Data
  M5.Lcd.setCursor(xLat2, y);
  sprintf(buf, "%.6f", gpsData.lat);
  M5.Lcd.print(buf);
  
  M5.Lcd.setCursor(xLon2, y);
  sprintf(buf, "%.6f", gpsData.lon);
  M5.Lcd.print(buf);
  
  unsigned long elapsedTime = 0;
  double distance = 0.0;
  
  if (startPosition.isSet && gpsData.isValid) {
    elapsedTime = (millis() - startPosition.time) / 1000;
    distance = TinyGPSPlus::distanceBetween(startPosition.lat, startPosition.lon,
                                            gpsData.lat, gpsData.lon);
  }
  
  M5.Lcd.setCursor(xDauer, y);
  sprintf(buf, "%lu", elapsedTime);
  M5.Lcd.print(buf);
  
  M5.Lcd.setCursor(xDist, y);
  sprintf(buf, "%.1f", distance);
  M5.Lcd.print(buf);
  y += 15;
}

void displayAccelerationTable(int &y, const IMUData &imu) {
  const int xAx = 5, xAy = 110, xAz = 215;
  char buf[32];
  
  // Headers
  M5.Lcd.setCursor(xAx, y); M5.Lcd.print("x-Accel");
  M5.Lcd.setCursor(xAy, y); M5.Lcd.print("y-Accel");
  M5.Lcd.setCursor(xAz, y); M5.Lcd.print("z-Accel");
  y += 10;
  
  M5.Lcd.setCursor(xAx, y); M5.Lcd.print("(mg)");
  M5.Lcd.setCursor(xAy, y); M5.Lcd.print("(mg)");
  M5.Lcd.setCursor(xAz, y); M5.Lcd.print("(mg)");
  y += 7;
  
  drawSeparator(y);
  y += 10;
  
  // Data
  M5.Lcd.setCursor(xAx, y);
  sprintf(buf, "%d", (int)(imu.ax * 1000.0f));
  M5.Lcd.print(buf);
  
  M5.Lcd.setCursor(xAy, y);
  sprintf(buf, "%d", (int)(imu.ay * 1000.0f));
  M5.Lcd.print(buf);
  
  M5.Lcd.setCursor(xAz, y);
  sprintf(buf, "%d", (int)(imu.az * 1000.0f));
  M5.Lcd.print(buf);
  y += 15;
}

void displayRoadCondition(int &y, const IMUData &imu) {
  M5.Lcd.setCursor(5, y);
  M5.Lcd.setTextColor(getRoadConditionColor(imu.magnitude), BLACK);
  M5.Lcd.print(getRoadCondition(imu.magnitude));
  M5.Lcd.setTextColor(WHITE, BLACK);
  y += 10;
  
  M5.Lcd.setCursor(5, y);
  M5.Lcd.setTextColor(CYAN, BLACK);
  M5.Lcd.print("BtnA: Set Start");
  M5.Lcd.setTextColor(WHITE, BLACK);
  
  if (imu.magnitude > BUMP_THRESHOLD) {
    Serial.println(">>> BUMP DETECTED!");
  }
}

void updateDisplay(const GPSData &gpsData, const IMUData &imu) {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextFont(1);
  
  int y = 5;
  displayHeader(y);
  displayGPSTable(y, gpsData);
  displayPositionTable(y, gpsData);
  displayAccelerationTable(y, imu);
  displayRoadCondition(y, imu);
  
  Serial.printf("GPS: %.6f, %.6f | IMU: %.3f g\n", gpsData.lat, gpsData.lon, imu.magnitude);
}

void sendToInfluxDB() {
  sensorData.clearFields();
  
  // GPS data
  if (gps.location.isValid()) {
    sensorData.addField("latitude", gps.location.lat());
    sensorData.addField("longitude", gps.location.lng());
  }
  if (gps.satellites.isValid()) {
    sensorData.addField("satellites", gps.satellites.value());
  }
  if (gps.hdop.isValid()) {
    sensorData.addField("hdop", gps.hdop.hdop());
  }
  if (gps.speed.isValid()) {
    sensorData.addField("speed", gps.speed.kmph());
  }
  
  // IMU data
  IMUData currentIMU = readIMU();
  sensorData.addField("acc_x", currentIMU.ax);
  sensorData.addField("acc_y", currentIMU.ay);
  sensorData.addField("acc_z", currentIMU.az);
  sensorData.addField("acc_magnitude", currentIMU.magnitude);
  
  // Distance from start
  if (startPosition.isSet && gps.location.isValid()) {
    double distance = TinyGPSPlus::distanceBetween(
      startPosition.lat, startPosition.lon,
      gps.location.lat(), gps.location.lng()
    );
    sensorData.addField("distance_from_start", distance);
  }
  
  // Send
  if (client.writePoint(sensorData)) {
    Serial.println("✓ Data sent to InfluxDB");
  } else {
    Serial.printf("✗ InfluxDB write failed: %s\n", client.getLastErrorMessage().c_str());
  }
}

// ===== Main Functions =====
void setup() {
  initializeHardware();
  initializeDisplay();
  initializeGPS();
  connectWiFi();
  initializeInfluxDB();
  
  delay(2000);
  Serial.println("\nSystem Ready!");
  Serial.println("Press Button A to set start position");
}

void loop() {
  M5.update();
  handleButtonPress();
  processGPSData();
  
  IMUData imu = readIMU();
  
  if (millis() - lastUpdate > DISPLAY_UPDATE_INTERVAL) {
    lastUpdate = millis();
    GPSData gpsData = getGPSData();
    updateDisplay(gpsData, imu);
  }
  
  if (WiFi.status() == WL_CONNECTED && millis() - lastInfluxSend > INFLUX_SEND_INTERVAL) {
    lastInfluxSend = millis();
    sendToInfluxDB();
  }
}
