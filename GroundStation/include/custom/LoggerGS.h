#pragma once
#include <Arduino.h>
#include <map>
#include <ConsoleRouter.h>

//For logging to work the console router must be initialised
#define LOGGING(tag, msg) LoggerGS::getInstance().log(tag, msg)

class LoggerGS
{
public:
    static LoggerGS &getInstance();
    void configure(const std::map<String, bool> &config);
    void setEnabled(const String &tag, bool state);
    bool isEnabled(const String &tag) const;

    template <typename T>
    void log(const String &tag, const T &msg)
    {
        if (isEnabled(tag))
        {
            Console.println(msg);
        }
    }

private:
    LoggerGS();                
    static LoggerGS *instance; 

    std::map<String, bool> enabledTags;
};
