#pragma once

#include <Arduino.h>

#include "ConsoleRouter.h"
#include "LoggerConfig.h"

// For logging to work above PIPE level, the console router must be initialized
#define LOGGING(level, msg) LoggerGS::getInstance().log(level, msg)

class LoggerGS
{
public:
    static LoggerGS &getInstance();

    // Configure the log level for the system
    void configure(LogLevel level);

    // Set the global log level
    void setLogLevel(LogLevel level);

    // Get the global log level
    LogLevel getLogLevel() const;

    // Log method to handle messages at different log levels
    template <typename T>
    void log(LogLevel level, const T &msg);

private:
    LoggerGS();

    LogLevel globalLogLevel = GS_LOG_LEVEL;
};

template <typename T>
void LoggerGS::log(LogLevel level, const T &msg)
{
    // Only log if level >= global log level
    if (level >= getLogLevel())
    {
        if (level == PIPE)
        {
            // PIPE level is for serial only logs
            Serial.println(msg);
        }
        else
        {
            Console.println(msg);
        }
    }
}