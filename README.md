# 📷 M5Stack Camera Monitor
A portable Wi-Fi camera viewer implemented on the **M5Stack Core ESP32**.  
The device connects directly to an ESP32-CAM access point, fetches JPEG frames via HTTP, and displays them live on the M5Stack LCD.

---

## 🚀 Features
- Connects to ESP32-CAM Wi-Fi AP (no router required)
- Downloads JPEG frames from `/capture`
- Full-screen 320×240 image display on M5Stack
- Smooth, low-latency pseudo-video stream
- Simple PlatformIO build and upload workflow

---

## 🛠 Hardware Requirements
- M5Stack Core (classic ESP32 model)
- ESP32-CAM or M5Stack Camera Unit (OV2640)
- USB-C data cable (not charging-only)
