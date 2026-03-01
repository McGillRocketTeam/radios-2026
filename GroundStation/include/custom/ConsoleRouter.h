#pragma once

#include <Arduino.h>
#include <IntervalTimer.h>

#include "CommandParser.h"
#include "MqttTopics.h"

#define Console ConsoleRouter::getInstance()

class ConsoleRouter {
public:
    // Singleton accessor
    static ConsoleRouter &getInstance();

    // Verify if .begin has been called
    static bool isReady();

    // Init Serial, Ethernet Physical Link, MQTT Digital Link
    void begin(MqttTopic::Role role,CommandParser& parser);

    // Call in loop() to (re)connect Ethernet/MQTT
    void handleConsoleReconnect();

    // Call in loop() to keep connection MQTT alive
    void mqttLoop();

    // Stream API methods linked to serial debuging

    // Availabillity of serial, mqtt is checked seperately
    int     available();

    // Read from Serial
    int     read();

    //Peak from Serial
    int     peek();
    
    // Write to Serial and MQTT
    size_t  write(uint8_t c);
    size_t  write(const uint8_t *buffer, size_t size);

    // Sending serialised telemetry frame buffer via MQTT
    void sendTelemetry(const uint8_t *buffer, size_t size);

    // Sending serialised gs_rssi,gs_snr of the corresponding telemetry frame
    void sendRadioTelemetry(const uint8_t *buffer, size_t size);

    // Sending status of radio over MQTT
    void sendStatus();

    // Sending ack for rx of cmd from GSC
    void sendCmdAckRx(const String& s);

    // Sending ack after tx of cmd from CTS to GSC
    void sendCmdAckTx(int id);

    // Print overloads via templating
    template <typename T> void print(const T& v)   { _printString(String(v), false); }
    template <typename T> void println(const T& v) { _printString(String(v), true ); }
    
    // Supported specific types
    void print(const char* s);
    void println(const char* s);
    void print(const String& s);
    void println(const String& s);
    void print(char c);
    void println();

    void print(const __FlashStringHelper* fs);
    void println(const __FlashStringHelper* fs);

private:

    ConsoleRouter() = default;
    ConsoleRouter(const ConsoleRouter&) = delete;
    ConsoleRouter& operator=(const ConsoleRouter&) = delete;

    // Sets the mqtt topics and host name via role and band
    void applyRoleConfig(MqttTopic::Role role);

    // Ethernet ISR to set ethernet reconnect check flag
    static void ethernetCheckISR();

    // Try to connect to the server
    // will search the ip list and send a status if succesful
    bool mqttReconnect();

    // Internal helper for cmd acks
    void publishAck(const char* status, uint8_t ackId);

    void publish(const char* topic, const uint8_t* payload, unsigned int length);
    void publishRetained(const char* topic, const uint8_t* payload, unsigned int length);

    // Grabbing the mac address of the teensy
    void getMac(uint8_t mac[6]);

    // Command parser injection
    CommandParser* commandParser = nullptr;

    // Timer for periodic link checks
    IntervalTimer ethernetTimer;

    const char* deviceName_              = nullptr;
    
    const char* rocketTelemetryTopic_    = nullptr;
    const char* ackTopic_                = nullptr;
    const char* commandTopic_            = nullptr;

    const char* radioTelemetryTopic_     = nullptr;
    const char* statusTopic_             = nullptr;
    const char* detailTopic_             = nullptr;
    const char* debugTopic_              = nullptr;
    
    // Some safe defaults, but band and role needs to be set dynamically on startup
    MqttTopic::Band band_ = MqttTopic::Band::A;
    MqttTopic::Role role_ = MqttTopic::Role::CS;

    // Internal helpers for printing/publishing
    void _printString(const String& s, bool newline);
    void _publishBytes(const uint8_t* data, size_t len);

};

// Global flag used by ISR (defined in .cpp)
extern volatile bool ethernetReconnectNeeded;
