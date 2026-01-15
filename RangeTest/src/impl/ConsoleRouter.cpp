#include "ConsoleRouter.h"
#include <Config.h>
#include <PinLayout.h>

// === Setup ===

ConsoleRouter::ConsoleRouter() {}

void ConsoleRouter::begin()
{
    Serial.begin(GS_SERIAL_BAUD_RATE);
}

ConsoleRouter &ConsoleRouter::getInstance()
{
    static ConsoleRouter instance;
    return instance;
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
    // Need to modify to fit testing case
    Serial.println("This should be some sort of log");
}

void ConsoleRouter::sendStatus()
{
    // Stub this function for test
    return;
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
    if (!newline)
    {
        Serial.print(s);
    }
    else
    {
        Serial.println(s);
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
