# 📷 Versuch-CameraServer

**IoT / Industry 4.0 Internship – Experiment 1: Front Door Surveillance**

This project implements **Experiment 1** from the IoT / Industry 4.0 laboratory course at Hochschule Osnabrück.
The goal is to build a **front-door surveillance system** using:

- **M5Stack Timer Camera** (ESP32-CAM)
- **M5Stack Core ESP32**

Part 1 focuses on bringing up the camera as a webserver.
Part 2 displays the live camera stream on the M5Stack display.

This repository contains the source code developed as part of the experiment.

---

# 🧩 Hardware

- **M5Stack Timer Camera (ESP32-CAM)**
- **M5Stack Core ESP32**
- USB-C cable
- Optional: smartphone or PC for camera stream testing

---

# 🛠 Software Setup (Part 1)

### 1. Flash the M5Stack Timer Camera

Use the provided PlatformIO project:

```
Versuch_1_CameraWebserver
```

Unzip → open in PlatformIO → upload to the camera.

Set the operating mode:

```cpp
#define RUN_IN_AP_MODE true
```

If `true` → camera runs as Access Point (recommended for lab).
If `false` → camera connects to an existing Wi-Fi network.

Configure SSID + password:

```cpp
const char* ssid = "M5Stack_AP_Gruppe_X";
const char* password = "Password";
```

After flashing, open the serial monitor to read the **IP address**.

---

### 2. Test the Video Stream

Open the browser on smartphone/PC:

```
http://<camera-ip>
```

or direct image access:

```
http://<camera-ip>:80/capture
```

Close the browser afterwards — the camera **cannot serve multiple clients simultaneously**.

---

### 3. Implement M5Stack Program

Create a new PlatformIO project for the M5Stack Core.

Include required libraries:

```cpp
#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>
```

Your code must:

- Connect to the camera’s AP
- Send HTTP GET to `/capture`
- Read image bytes using `getStreamPtr()->readBytes()`
- Allocate buffer using `malloc()`
- Render JPEG via:

  ```cpp
  M5.Lcd.drawJpg(buffer, size, 0, 0, 320, 240);
  ```

- Handle errors (connection failures, HTTP errors, memory issues)

---

# 🔧 Serial Port Troubleshooting (macOS, 99% success)

Because the M5Stack creates **two** `/dev/cu.usbserial-*` devices:

### The correct upload port is **the one that prints output when you press RESET**.

#### 1. Unplug → check ports:

```bash
ls /dev/cu.*
```

#### 2. Plug in → check again:

```bash
ls /dev/cu.*
```

New entries appear, e.g.:

```
/dev/cu.usbserial-05523F73A9
/dev/cu.usbserial-56E20458921
```

#### 3. Test each port:

```bash
pio device monitor --port /dev/cu.usbserial-XXXX -b 115200
```

Press **RESET** on the M5Stack.

- If output appears → **correct port**
- If silent → wrong port

Use this port in `platformio.ini`:

```ini
upload_port = /dev/cu.usbserial-56E20458921
```

---

# 📁 Repository Contents

```
Versuch-cameraserver/
│
├── src/
│   └── main.cpp          # M5Stack program
│
├── platformio.ini        # Build configuration
│
└── README.md             # Documentation (this file)
```

---

# 📄 License

Academic use — for IoT/Industry 4.0 laboratory coursework.

---

# 👤 Author

Achraf Bennani
Hochschule Osnabrück – IoT / Industry 4.0 Laboratory
