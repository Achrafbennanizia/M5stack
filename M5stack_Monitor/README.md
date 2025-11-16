# 📷 M5Stack Camera Monitor

_A Wi-Fi camera viewer running directly on the M5Stack Core (ESP32)_

This project connects a **M5Stack Core ESP32** to the Wi-Fi access point of an **ESP32 Camera module** and displays the live JPEG stream directly on the M5Stack screen in real time.

It functions as a **portable wireless monitor**, useful for:

- viewing an ESP32-CAM feed without a phone or PC
- checking camera alignment during robotics projects
- debugging camera modules on the go
- creating low-cost IoT visual inspection tools

The project is built using **PlatformIO + Arduino Framework**.

---

# 🚀 Features

### ✔️ Connects directly to the camera’s Wi-Fi Access Point

The camera broadcasts its own AP (e.g., `AP_M5STACK_CAM_Moo`).
The M5Stack joins this network and establishes an HTTP connection.

### ✔️ Fetches live images over HTTP (`/capture`)

The camera serves a JPEG snapshot at:

```
http://192.168.4.1/capture
```

The M5Stack downloads this image repeatedly to create a pseudo live-view.

### ✔️ Displays JPEG frames on the M5Stack LCD

Images are rendered using:

```cpp
M5.Lcd.drawJpg(buffer, bytesRead, 0, 0, 320, 240);
```

which decodes the JPEG directly onto the screen.

### ✔️ Full-screen 320×240 display support

Fits the M5Stack Core LCD resolution.

---

# 📦 Project Structure

```
M5stack_Monitor/
│
├── src/
│   └── main.cpp          # Main application
│
├── platformio.ini        # PlatformIO config
│
└── README.md             # Documentation (this file)
```

---

# 🛠 Hardware Requirements

- **M5Stack Core (ESP32)**
  (classic model with 3 buttons)
- **ESP32-CAM module** or
  **M5Stack Camera Unit (OV2640)**
- USB-C cable (must support DATA, not charging-only)

---

# 🧩 Software Requirements

- PlatformIO Core (CLI)
  or PlatformIO VS Code extension
- CP210x USB-UART driver (macOS)
  [https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)

---

# ⚙️ Installation

### 1️⃣ Clone the project

```bash
git clone https://github.com/Achrafbennanizia/M5stack.git
cd M5stack_Monitor
```

---

### 2️⃣ Verify `platformio.ini`

```ini
[env:m5stack-core-esp32]
platform = espressif32
board = m5stack-core-esp32
framework = arduino
monitor_speed = 115200

upload_port = /dev/cu.usbserial-56E20458921   ; <-- replace with YOUR correct port

lib_deps =
    m5stack/M5Stack
```

---

### 🔍 **How to Identify the Correct Serial Port (99% reliable method)**

Since macOS often creates **two** `/dev/cu.usbserial-*` ports, use this procedure to find the real ESP32 upload port:

#### **1️⃣ Unplug the M5Stack**

Run:

```bash
ls /dev/cu.*
```

Remember which ports are present.

#### **2️⃣ Plug the M5Stack back in**

Run again:

```bash
ls /dev/cu.*
```

You will now see **one or two new entries**, for example:

```
/dev/cu.usbserial-05523F73A9
/dev/cu.usbserial-56E20458921
```

These are your ESP32 ports.

#### **3️⃣ Test each port with the serial monitor**

Try the first port:

```bash
pio device monitor -b 115200 --port /dev/cu.usbserial-05523F73A9
```

Now press the **RESET** button on the M5Stack.

- If you see **boot messages or program output**, this is the correct port.
- If the monitor stays **silent**, close it with `Ctrl + C` and test the second port:

```bash
pio device monitor -b 115200 --port /dev/cu.usbserial-56E20458921
```

Again, press **RESET**.

#### ⭐ **The port that shows output when pressing RESET is ALWAYS the correct upload port.**

Use that port in your `platformio.ini`:

```ini
upload_port = /dev/cu.usbserial-56E20458921
```

---

### 3️⃣ Configure camera Wi-Fi in `main.cpp`

```cpp
const char* ssid = "AP_M5STACK_CAM_Gruppe_X";
const char* password = "12345678";
const char* cameraURL = "http://192.168.4.1:80/capture";  // <-- camera snapshot endpoint
```

---

### 4️⃣ Upload the firmware

```bash
pio run -t upload
```

---

### 5️⃣ Open the serial monitor

```bash
pio device monitor -b 115200 --port /dev/cu.usbserial-56E20458921
```

---

# 📺 How it Works (Technical Overview)

1. **Initialize M5Stack**
   LCD, power, and serial interface.

2. **Connect to the camera’s Wi-Fi AP**

```cpp
WiFi.begin(ssid, password);
```

3. **Perform HTTP GET on `/capture`**

```cpp
http.begin(cameraURL);
int httpCode = http.GET();
```

4. **Allocate buffer and read JPEG**

```cpp
uint8_t* buffer = (uint8_t*)malloc(len);
stream->readBytes(buffer, len);
```

5. **Display JPEG frame**

```cpp
M5.Lcd.drawJpg(buffer, bytesRead, 0, 0);
```

6. **Repeat every 100ms**
   Creates a 10-fps pseudo-video stream.

---

# 🧪 Debugging Guide (Detailed & Tested)

This section is based on actual debugging steps we solved.

---

## 🟧 **1. Port Busy / Cannot Open Serial Port**

```
Could not open /dev/cu.usbserial-XXXX, the port is busy
```

### Fix:

Close all serial monitors:

```
Ctrl + C
```

Or restart terminal.

Only one tool may use the port at a time.

---

## 🟧 **2. Two USB ports appear (`05523F73A9` and `56E20458921`)**

Example:

```
/dev/cu.usbserial-05523F73A9
/dev/cu.usbserial-56E20458921
```

### Solution:

The **real ESP32 port** is the one that prints boot logs:

```
pio device monitor --port /dev/cu.usbserial-56E20458921
```

If it prints:

```
WiFi Failed!
Waiting for GPS fix...
```

→ This is the correct upload port.

---

## 🟧 **3. No serial data received during upload**

```
A fatal error occurred: Unable to verify flash chip connection
```

### Fix:

#### ① Use correct port

```ini
upload_port = /dev/cu.usbserial-56E20458921
```

#### ② Enter bootloader mode

1. Hold **Button A** (left button)
2. Press RESET
3. Release RESET but keep holding A 2 seconds
4. Run upload:

```bash
pio run -t upload
```

#### ③ Use a real USB data cable

Cheap charging cables won’t upload.

#### ④ Install CP210x driver

(macOS only)

---

## 🟧 **4. Camera does not connect**

Check SSID and password in `main.cpp`.

---

## 🟧 **5. Black screen / No image shown**

Usually means:

- wrong camera URL
- camera not powered
- buffer allocation failed

Check serial output for:

```
HTTP Error: ...
Memory allocation failed!
```

---

# 🎉 Result

You now have a fully working **standalone M5Stack camera monitor** capable of receiving and displaying JPEG frames from an ESP32-CAM over Wi-Fi.
