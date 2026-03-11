#include "CommandParser.h"

#include <Arduino.h>
#include <cstring>
#include <cstdlib>
#include <cstddef>

#include "Config.h"
#include "GroundCommand.h"
#include "LoggerGS.h"
#include "RingQueue.h"
#include "RocketCommand.h"

// === Setup ===

CommandParser &CommandParser::getInstance()
{
    static CommandParser inst;
    return inst;
}

CommandParser::CommandParser()
    : groundCommandQueue(),
      rocketCommandQueue()
{
    currentLine.buf[0] = '\0';
    currentLine.len = 0;
}

// === MAIN public functions ===

void CommandParser::update()
{
    while (Serial.available())
    {
        char c = (char)Serial.read();
        if (c == '\r')
        {
            continue;
        }

        if (c == '\n')
        {
            if (currentLine.len == 0)
                continue;
            currentLine.buf[currentLine.len] = '\0';
            enqueueCommand(currentLine);

            currentLine.len = 0;
            currentLine.buf[0] = '\0';
            continue;
        }

        if (c == '\b' || c == 127)
        {
            handleBackspace();
            continue;
        }

        if (isPrintableCharacter(c))
        {
            handleCharacterAppend(c);
            continue;
        }
    }
}

bool CommandParser::getNextGroundCommand(GroundCommand::Cmd &out)
{
    return groundCommandQueue.dequeue(out);
}

bool CommandParser::getNextRocketCommand(command_packet_extended &outPkt)
{
    return rocketCommandQueue.dequeue(outPkt);
}

bool CommandParser::enqueueCommand(const command_line &line)
{
    LOGGING(CAT_PARSER, INFO, "enqueue called ");
    LOGGING(CAT_PARSER, INFO, line.buf);
    if (GroundCommand::isGroundCommand(line))
    {
        if (groundCommandQueue.isFull()) {
            LOGGING(CAT_PARSER, INFO, "ground command queue full, discarding old, enqueuing new");
        }
        
        GroundCommand::Cmd groundCommand = GroundCommand::parseGroundCmd(line);
        if (groundCommand.action != GroundCommand::Action::Unknown)
        {
            groundCommandQueue.enqueue(groundCommand);
        }
        else
        {
            LOGGING(CAT_PARSER, INFO, "Bad ground command");
            return false;
        }
        return true;
    }

    command_packet_extended rocketCommand = {};
    if (RocketCommand::fillCommandPacket(line, rocketCommand))
    {
        if (rocketCommandQueue.isFull()) {
            LOGGING(CAT_PARSER, INFO, "rocket command queue full, discarding old, enqueuing new");
        }
        rocketCommandQueue.enqueue(rocketCommand);
        return true;
    }
    else
    {
        LOGGING(CAT_PARSER, INFO, "Bad rocket command");
        return false;
    }
}

bool CommandParser::isRocketQueueFull(){
    return rocketCommandQueue.isFull();
}

void CommandParser::clearRocketQueue()
{
    printRocketQueueStatus();
    command_packet_extended tmp;
    
    while (rocketCommandQueue.dequeue(tmp)) {}
    printRocketQueueStatus();
}
// === Public debug functions ===

void CommandParser::printGroundQueueStatus()
{
    char buf[32];
    snprintf(buf, sizeof(buf), "Ground queue has %d items", groundCommandQueue.itemCount());
    LOGGING(CAT_PARSER, PIPE, buf);
}

void CommandParser::printRocketQueueStatus()
{
    char buf[32];
    snprintf(buf, sizeof(buf), "Rocket queue has %d items", rocketCommandQueue.itemCount());
    LOGGING(CAT_PARSER, PIPE, buf);
}

// === String and char helpers ===
void CommandParser::handleBackspace()
{
    if (currentLine.len <= 0)
        return;
    // We could also set null terminator, but before we send
    // the command we always null terminate so its ok
    currentLine.len--;
    if (LoggerGS::getInstance().getLogLevel() == PIPE)
    {
        Serial.print("\b \b");
    }
}

bool CommandParser::isPrintableCharacter(char c)
{
    return c >= 32 && c <= 126;
}

void CommandParser::handleCharacterAppend(char c)
{
    if (currentLine.len < (MAX_COMMAND_LENGTH - 1))
    {
        currentLine.buf[currentLine.len++] = c;
        if (LoggerGS::getInstance().getLogLevel() == PIPE)
            Serial.print(c);
    }
    else
    {
        LOGGING(CAT_PARSER, PIPE, "Warning: Command too long, ignoring extra characters.");
    }
}
