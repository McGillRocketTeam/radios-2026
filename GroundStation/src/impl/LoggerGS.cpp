#include "LoggerGS.h"

LoggerGS::LoggerGS() {}

LoggerGS& LoggerGS::getInstance() {
    static LoggerGS instance;
    return instance;
}

// --- Severity ---
void LoggerGS::setLogLevel(LogLevel level) {
    globalLogLevel = level;
}

LogLevel LoggerGS::getLogLevel() const {
    return globalLogLevel;
}

// --- Categories ---
void LoggerGS::setCategoryMask(uint32_t mask) {
    enabledCats = mask;
}

uint32_t LoggerGS::getCategoryMask() const {
    return enabledCats;
}
