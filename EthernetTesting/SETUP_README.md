# 🧩 MQTT Setup Guide

## Prerequisites
- **Operating System:** Linux, macOS, or Windows  
- **Required tools:**  
  - [Eclipse Mosquitto](https://mosquitto.org/download/) — MQTT Broker  
  - [MQTT Explorer](https://mqtt-explorer.com/) — GUI Client for MQTT

---


## 1. Connect to MRT-GS modem wirelessly
password: H0tFire!

## 2. Start Mosquitto Broker

### Default configuration
```bash
mosquitto -c mosquitto.conf -v
```

## 3.  Connect MQTT Explorer to broker

Protocal: mqtt://
Host: localhost (or your server IP)
Port: 1883 (default)

Need to verify that you firewall is not blocking connection attempts

## 4. Setup Teensy 4.1 

Connect teensy 4.1 physically to modem LAN port
Push main.cpp into teensy 4.1