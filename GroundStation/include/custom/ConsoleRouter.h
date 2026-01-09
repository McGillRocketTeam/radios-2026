#pragma once
#include <Arduino.h>
#include <IntervalTimer.h>

#define Console ConsoleRouter::getInstance()

class ConsoleRouter {
public:
    // Singleton accessor
    static ConsoleRouter &getInstance();

    // Init Serial, Ethernet Physical Link, MQTT Digital Link
    void begin();

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

    // Sending serialised telemetry buffer via MQTT
    void sendTelemetry(const uint8_t *buffer, size_t size);

    // Sending status of radio over MQTT
    void sendStatus();

    template <typename T> void print(const T& v)   { _printString(String(v), false); }
    template <typename T> void println(const T& v) { _printString(String(v), true ); }
    
    void print(const char* s);
    void println(const char* s);
    void print(const String& s);
    void println(const String& s);
    void print(char c);
    void println();

    // PROGMEM strings like F("text")
    void print(const __FlashStringHelper* fs);
    void println(const __FlashStringHelper* fs);

private:

    size_t  write(uint8_t c);
    size_t  write(const uint8_t *buffer, size_t size);

    ConsoleRouter();
    ConsoleRouter(const ConsoleRouter&) = delete;
    ConsoleRouter& operator=(const ConsoleRouter&) = delete;

    // Checks the pins and sets the topics based on the frequency
    void setTopicsFromPins();

    // Ethernet bring-up and periodic check ISR
    void        ethernetInit();
    static void ethernetCheckISR();

    // Timer for periodic link checks
    IntervalTimer ethernetTimer;
    
    const char* metadataTopic;
    const char* telemetryTopic;
    const char* debugTopic;


    // Internal helpers for printing/publishing
    void _printString(const String& s, bool newline);
    void _publishBytes(const uint8_t* data, size_t len);

};

// Global flag used by ISR (defined in .cpp)
extern volatile bool ethernetReconnectNeeded;
