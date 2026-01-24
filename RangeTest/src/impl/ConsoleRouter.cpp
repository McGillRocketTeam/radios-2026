#include "ConsoleRouter.h"
#include <Config.h>
#include <PinLayout.h>

// === Setup ===

ConsoleRouter::ConsoleRouter() : mqttClient(ethernetClient) {}

void ConsoleRouter::begin()
{
    Serial.begin(GS_SERIAL_BAUD_RATE);
    ethernetInit();
    _connectMQTT();
}

ConsoleRouter &ConsoleRouter::getInstance()
{
    static ConsoleRouter instance;
    return instance;
}

void ConsoleRouter::ethernetInit()
{
    // Initialize Ethernet with DHCP
    byte mac[] = {0x04, 0xE9, 0xE5, 0x12, 0x34, 0x56};
    Ethernet.begin(mac);
    
    Serial.println("[ETH] Ethernet initialized with DHCP");
}

void ConsoleRouter::_connectMQTT()
{
    mqttClient.setServer(MQTT_BROKER_IP, MQTT_BROKER_PORT);
    
    if (mqttClient.connect(MQTT_CLIENT_ID))
    {
        mqttConnected = true;
        Serial.print("[MQTT] Connected to broker at ");
        Serial.println(MQTT_BROKER_IP);
    }
    else
    {
        Serial.print("[MQTT] Failed to connect, rc=");
        Serial.println(mqttClient.state());
    }
}

void ConsoleRouter::handleConsoleReconnect()
{
    // Check Ethernet connection
    if (Ethernet.linkStatus() == LinkOFF)
    {
        Serial.println("[ETH] Link down, attempting to reconnect...");
        Ethernet.maintain();
    }
    
    // Reconnect MQTT if disconnected
    if (!mqttClient.connected())
    {
        if (Ethernet.linkStatus() == LinkON)
        {
            Serial.println("[MQTT] Attempting to reconnect...");
            _connectMQTT();
        }
    }
}

void ConsoleRouter::mqttLoop()
{
    if (mqttClient.connected())
    {
        mqttClient.loop();
    }
}

int ConsoleRouter::available()
{
    // Only Serial is readable; MQTT is publish-only
    return Serial.available();
}

int ConsoleRouter::read()
{
    if (Serial.available() > 0)
        return Serial.read();
    return -1;
}

int ConsoleRouter::peek()
{
    if (Serial.available() > 0)
        return Serial.peek();
    return -1;
}

void ConsoleRouter::sendTelemetry(const uint8_t *buffer, size_t size)
{
    // Publish binary telemetry data to MQTT
    if (mqttClient.connected())
    {
        mqttClient.publish(MQTT_TELEMETRY_TOPIC, (const byte*)buffer, size);
    }
}

void ConsoleRouter::sendStatus()
{
    // Publish status message to MQTT
    if (mqttClient.connected())
    {
        mqttClient.publish(MQTT_STATUS_TOPIC, "OK");
    }
}

size_t ConsoleRouter::write(uint8_t c)
{
    return Serial.write(c);
}

size_t ConsoleRouter::write(const uint8_t *buffer, size_t size)
{
    // Always mirror to USB serial
    Serial.write(buffer, size);
    return size;
}

void ConsoleRouter::_printString(const String &s, bool newline)
{
    // Always print to Serial
    if (!newline)
    {
        Serial.print(s);
    }
    else
    {
        Serial.println(s);
    }
    
    // Publish to MQTT if connected
    if (mqttClient.connected())
    {
        String message = s;
        if (newline)
        {
            message += "\n";
        }
        mqttClient.publish(MQTT_LOGS_TOPIC, message.c_str());
    }
}

void ConsoleRouter::print(const char *s)
{
    if (!s)
        return;
    Serial.print(s);
}
void ConsoleRouter::println(const char *s)
{
    if (!s)
    {
        println();
        return;
    }
    Serial.println(s);
}

// --- String ---
void ConsoleRouter::print(const String &s) { _printString(s, false); }
void ConsoleRouter::println(const String &s) { _printString(s, true); }

// --- single char ---
void ConsoleRouter::print(char c)
{
    Serial.print(c);
}
void ConsoleRouter::println()
{
    Serial.println();
}

void ConsoleRouter::print(const __FlashStringHelper *fs)
{
    if (!fs)
        return;
    Serial.print(fs);
}
void ConsoleRouter::println(const __FlashStringHelper *fs)
{
    print(fs);
    println();
}
