#pragma once
#include <Arduino.h>
#include <Config.h>

#if TEENSY_BOARD_VERSION == 41
#include <NativeEthernet.h>
#endif

// Gets the console router instance, a substitute for standard Serial calls
#define Console ConsoleRouter::getInstance()

class ConsoleRouter : public Stream
{
private:
    #if TEENSY_BOARD_VERSION == 41
        EthernetClient client;
        IntervalTimer ethernetTimer;
    #endif
    ConsoleRouter();

public:
    static ConsoleRouter &getInstance();

    void begin();
    /*
    Checks for physical connection and if LinkOn tries to connect
    */
    static void ethernetCheckISR();
    void ethernetInit();
    // Tries to reconnect to the Ethernet client if there is a link
    void handleConsoleReconnect();
    int available() override;
    int read() override;
    int peek() override;
    size_t write(uint8_t c) override;
    size_t write(const uint8_t *buffer, size_t size);

    /*
    Printer to debug the console, only serial enabled
    */
    template <typename T>
    void debugPrint(const T &msg)
    {
        if (CONSOLE_ROOT_DEBUG)
        {
            Serial.println(msg);
        }
    }

    int ethernetAvailble(){
        #if TEENSY_BOARD_VERSION == 41
            return client.available();
        #endif
        return -1;
    }

    bool checkEthernetConnected() {
        #if TEENSY_BOARD_VERSION == 41
        return (Ethernet.linkStatus() == LinkON) && client.connected();
        #else
        return false;
        #endif
    }

    template <typename T>
    void println(const T &msg)
    {
        Serial.println(msg);
        if (checkEthernetConnected())
        {
            #if TEENSY_BOARD_VERSION == 41
            client.println(msg);
            #endif
            
        }
    }

    void println()
    {
        Serial.println();
        if (checkEthernetConnected())
        {
            #if TEENSY_BOARD_VERSION == 41
            client.println();
            #endif
        }
    }

    template <typename T>
    void print(const T &msg)
    {
        Serial.print(msg);
        if (checkEthernetConnected())
        {
            #if TEENSY_BOARD_VERSION == 41
            client.print(msg);
            #endif
        }
    }

    

    void print(const __FlashStringHelper *msg)
    {
        Serial.print(msg);
        if (checkEthernetConnected())
        {
            #if TEENSY_BOARD_VERSION == 41
            client.print(msg);
            #endif
        }
    }

    void println(const __FlashStringHelper *msg)
    {
        Serial.println(msg);
        if (checkEthernetConnected())
        {
            #if TEENSY_BOARD_VERSION == 41
            client.println(msg);
            #endif
        }
    }

    using Print::write; // Enables inherited write(buffer, size)
};
