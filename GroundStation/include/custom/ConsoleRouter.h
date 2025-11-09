#pragma once
#include <Arduino.h>
#include <IntervalTimer.h>

#define Console ConsoleRouter::getInstance()

class ConsoleRouter {
public:
    // Singleton accessor
    static ConsoleRouter &getInstance();

    // Init Serial, Ethernet, MQTT
    void begin();

    // Call regularly in loop() to (re)connect Ethernet/MQTT
    void handleConsoleReconnect();

    // Must be called often to keep MQTT alive
    void mqttLoop();

    // Console-like API (read from Serial, publish on write)
    int     available();
    int     read();
    int     peek();
    size_t  write(uint8_t c);
    size_t  write(const uint8_t *buffer, size_t size);

    template <typename T> void print(const T& v)   { _printString(String(v), false); }
    template <typename T> void println(const T& v) { _printString(String(v), true ); }
    // Common fast paths
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
    ConsoleRouter();
    ConsoleRouter(const ConsoleRouter&) = delete;
    ConsoleRouter& operator=(const ConsoleRouter&) = delete;

    // Ethernet bring-up and periodic check ISR
    void        ethernetInit();
    static void ethernetCheckISR();

    // Timer for periodic link checks
    IntervalTimer ethernetTimer;

    
    // Internal helpers for printing/publishing
    void _printString(const String& s, bool newline);
    void _publishBytes(const uint8_t* data, size_t len);

};

// Global flag used by ISR (defined in .cpp)
extern volatile bool ethernetReconnectNeeded;
