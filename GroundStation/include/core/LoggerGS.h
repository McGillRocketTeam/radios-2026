#pragma once
#include <Arduino.h>
#include "ConsoleRouter.h"
#include "LoggerConfig.h"

// Logging with CRIT level bypasses the category mask
#define LOGGING(cat, level, msg) LoggerGS::getInstance().log((cat), (level), (msg))

class LoggerGS
{
public:
    static LoggerGS &getInstance();

    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;

    void setCategoryMask(uint32_t mask);
    uint32_t getCategoryMask() const;

    void setCategory(LoggingCategory cat, bool desiredState);

    template <typename T>
    void log(uint32_t cat, LogLevel level, const T &msg);

private:
    LoggerGS();

    LogLevel globalLogLevel = GS_LOG_LEVEL;
    uint32_t enabledCats = GS_LOG_CATS;

    inline bool enabled(uint32_t cat, LogLevel level) const
    {
        if (level == CRIT)
            return true;
        if (level < globalLogLevel)
            return false;
        // CRIT logs always make it through
        return (enabledCats & cat) != 0;
    }

    inline bool serialOnly(uint32_t cat) const
    {
        return (cat & CAT_SERIAL) != 0;
    }
};

// Logging with CRIT level bypasses the category mask
template <typename T>
void LoggerGS::log(uint32_t cat, LogLevel level, const T &msg)
{
    if (!enabled(cat, level))
        return;

    if (serialOnly(cat))
    {
        Serial.println(msg);
        return;
    }
    if (!ConsoleRouter::isReady())
    {
        Serial.println(msg);
        return;
    }
    Console.println(msg);
}