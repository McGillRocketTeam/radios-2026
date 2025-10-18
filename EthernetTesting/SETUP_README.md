# 🧩 MQTT Setup Guide

## Prerequisites

- **Operating System:** Linux, macOS, or Windows
- **Required tools:**
  - [Eclipse Mosquitto](https://mosquitto.org/download/) — MQTT Broker
  - [MQTT Explorer](https://mqtt-explorer.com/) — GUI Client for MQTT

---

## 1. Connect to MRT-GS modem wirelessly

SSID: MRT-GS  
password: H0tFire!

## 2. Start Mosquitto Broker

If not done, add Mosquitto to your path ENVIRONMENT VARIABLE. (probably similar to C:\Program Files\mosquitto)

### Default configuration

```bash
mosquitto -c mosquitto.conf -v
```

The `mosquitto.conf` file holds the configs for the Mosquitto server:

```conf
listener 1883
allow_anonymous true
```

Once the MQTT broker gets started, connect your MQTT Explorer to it.

## 3. Connect MQTT Explorer to broker

In `main.cpp` make sure that if the Teensy tries to start an MQTT server that the correct IP address is used.
It will look like `IPAddress mqttServer(w, x, y, z);`. If you do need to go get the IP,
navigate to `192.168.8.1` in your browser (password: H0tFire!) and locate your device's MAC address, which is the address that you will use to start the MQTT server.

Protocol: mqtt://  
Host: localhost (or your server IP)  
Port: 1883 (default)

Need to verify that your firewall is not blocking connection attempts

## 4. Setup Teensy 4.1

1. Connect teensy 4.1 physically to modem LAN port
2. Push main.cpp into teensy 4.1
3. Monitor result in the serial monitor
