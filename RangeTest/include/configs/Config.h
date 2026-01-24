#pragma once

#include <Arduino.h>
#include "LoraParamConfig.h"
#include "LoggerConfig.h"


//This is a special debug outside of the logger specific to the console. 
constexpr int GS_SERIAL_BAUD_RATE = 115200;

// Controls whether this code is allowed to transmit
constexpr bool ENABLE_RADIO_TX = true;

// Controls whether human readable telemetry is spit out to serial
constexpr bool ENABLE_VERBOSE_TELMETRY_PACKET = false;

// Radio identity and keywording
constexpr const char *RADIO_CALL_SIGN = "VA2 JWL";
constexpr const char *RADIO_COMMAND_KEYWORD = "radio";

// MQTT Configuration
constexpr const char* MQTT_BROKER_IP = "PLACEHOLDER"; // is the computer's IP on the router
constexpr int MQTT_BROKER_PORT = 1883;
constexpr const char* MQTT_CLIENT_ID = "GroundStationClient";
constexpr const char* MQTT_LOGS_TOPIC = "groundstation/logs";
constexpr const char* MQTT_STATUS_TOPIC = "groundstation/status";
constexpr const char* MQTT_TELEMETRY_TOPIC = "groundstation/telemetry";


