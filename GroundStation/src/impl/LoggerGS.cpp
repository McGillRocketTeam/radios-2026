#include "LoggerGS.h"
#include "Config.h"

// To Do route the debug logs to a specific debug topic
// To Do add commands to be able to configure the appearence of each

LoggerGS &LoggerGS::getInstance()
{
    static LoggerGS instance;
    return instance;
}

LoggerGS::LoggerGS()
{
    std::map<String, bool> initialConfig = {
        {DEBUG_RADIO, ENABLE_RADIO_DEBUG_MODE},
        {DEBUG_PARSER, ENABLE_COMMAND_PARSER_QUEUE},
        {DEBUG_VERBOSE_PACKET, ENABLE_PRINT_VERBOSE_PACKET},
        {DEBUG_BOOLEANS_PACKET, ENABLE_TABBED_BOOLEANS_PACKET_PRINT},
        {DEBUG_GUI_PRINT, ENABLE_PRINT_TO_GUI},
        {DEBUG_GS,ENABLE_GS_PRINT},
        {DEBUG_FC_PACKET,ENABLE_FC_PACKET_PRINT}
    };
    configure(initialConfig);
}

void LoggerGS::configure(const std::map<String, bool> &config)
{
    enabledTags = config;
}

void LoggerGS::setEnabled(const String &tag, bool state)
{
    enabledTags[tag] = state;
}

bool LoggerGS::isEnabled(const String &tag) const
{
    auto it = enabledTags.find(tag);
    return it != enabledTags.end() ? it->second : false;
}

