#include "LoggerGS.h"

LoggerGS::LoggerGS() {}

LoggerGS &LoggerGS::getInstance() {
    static LoggerGS instance;
    return instance;
}

void LoggerGS::configure(LogLevel level) {
    globalLogLevel = level;
}

void LoggerGS::setLogLevel(LogLevel level) {
    globalLogLevel = level;
}

LogLevel LoggerGS::getLogLevel() const {
    return globalLogLevel;
}
