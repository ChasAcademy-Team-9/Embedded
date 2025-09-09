# Embedded - Chas Advance – Hardware Test Documentation

## Overview

This project documents how we connected and tested the hardware for our system, including Arduino and ESP32 communication. The goal is to make it easy to repeat tests and troubleshoot issues in the future.

## Hardware Setup

- **Arduino Uno R4 WiFi** is used to read sensor data.
- **ESP32-S3 Zero** receives data from Arduino via WiFi.

### Sensors Connected

- DHT11 (Temperature & Humidity)

#### Example Connections

| Sensor   | Arduino Pin | Power Pin |
|----------|-------------|-----------|
| DHT22    | 8           | 6V        |

## Communication

- Arduino reads sensor values and sends them via WiFi to the ESP32.
- ESP32 runs a web server and receives sensor data as JSON via HTTP POST requests.

### Code Used for Testing

- Arduino: See `Chas Advance Arduino/src/main.cpp` and related sensor code.
- ESP32: See `Chas Advance ESP32/src/main.cpp` and `wifiHandler.cpp`.

### Example Arduino Test

<img width="370" height="94" alt="image" src="https://github.com/user-attachments/assets/d40dbadd-e5e1-4883-ada1-372ca9e23db4" />
<br><br>

- DHT22: Worked as expected, reported temperature and humidity values.
- WiFi Communication: Data sent from Arduino was received by ESP32 and displayed in Serial Monitor.

### How to Repeat the Tests
1. Connect sensors to Arduino as shown in the wiring diagram.
2. Upload the test code to Arduino and ESP32 using PlatformIO.
3. Open Serial Monitor to verify sensor readings and communication.
4. Check that ESP32 prints received data and its IP address.

### Troubleshooting
- Make sure to use a 2.4 GHz WiFi network (ESP32 does not support 5 GHz).
- Double-check SSID and password in ESPSECRETS.h and ARDUINOSECRETS.h.
- If no IP address is shown, check WiFi connection and wiring.
