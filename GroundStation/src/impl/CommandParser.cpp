#include "CommandParser.h"

#include <Arduino.h>

#include "Config.h"
#include "ConsoleRouter.h"
#include "LoggerGS.h"
#include "RingQueue.h"

// === Setup ===

CommandParser &CommandParser::getInstance()
{
    static CommandParser inst;
    return inst;
}

CommandParser::CommandParser()
    : radioCommandQueue(),
      rocketCommandQueue(),
      currentLen(0)
{
    currentBuf[0] = '\0';
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
            if (currentLen == 0)
                continue;

            currentBuf[currentLen] = '\0';
            enqueueCommand(String(currentBuf));

            currentLen = 0;
            currentBuf[0] = '\0';
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

bool CommandParser::getNextRadioCommand(String &outCommand)
{
    return dequeueCommand(radioCommandQueue, outCommand);
}

bool CommandParser::getNextRocketCommand(command_packet &outPkt)
{
    String raw;
    if (!dequeueCommand(rocketCommandQueue, raw)) {
        return false;
    }

    int commaIdx = raw.indexOf(',');
    if (commaIdx <= 0 || commaIdx == (int)raw.length() - 1) {
        return false;
    }

    long id = raw.substring(0, commaIdx).toInt();
    if (id < 0 || id > 255) {
        return false;
    }
    outPkt.data.command_id = (uint8_t)id;

    const char* cmdp = raw.c_str() + commaIdx + 1;
    memset(outPkt.data.command_string, 0, sizeof(outPkt.data.command_string));
    strncpy(outPkt.data.command_string, cmdp, sizeof(outPkt.data.command_string) - 1);

    return true;
}

// === Public debug functions ===

void CommandParser::printRadioQueueStatus()
{
    printQueueStatus("Radio", radioCommandQueue);
}

void CommandParser::printRocketQueueStatus()
{
    printQueueStatus("Rocket", rocketCommandQueue);
}

void CommandParser::printQueueStatus(const char *queueType,
                                     RingQueue<String, MAX_QUEUE_SIZE> &queue)
{
    Console.print(queueType);
    Console.print(F(" commands in queue: "));
    Console.println(queue.itemCount());
}

// === Private helper functions ===

bool CommandParser::isPingCommand(const String &command)
{
    // COULD have a more robust check here
    return command == "ping";
}

bool CommandParser::isRadioCommand(const String &s)
{
    // Keyword + a trailing white space
    if (s.length() <= (int)RADIO_COMMAND_KEY_LEN)
        return false;
    if (!s.startsWith(RADIO_COMMAND_KEYWORD))
        return false;

    char next = s.charAt(RADIO_COMMAND_KEY_LEN);
    return next == ' ';
}

bool CommandParser::normalizeRocketCommand(const String &in, String &out)
{
    String s = in;
    s.trim();

    if (s.length() < 2)
        return false;

    // Parse 1..3 digit ID at the start
    int i = 0;
    int id = 0;
    int digits = 0;

    while (i < (int)s.length() && digits < 3)
    {
        char c = s.charAt(i);
        if (c < '0' || c > '9')
            break;
        id = id * 10 + (c - '0');
        i++;
        digits++;
    }
    // must start with at least 1 digit
    if (digits == 0)
        return false;

    // Optional separators after the ID
    while (i < (int)s.length())
    {
        char c = s.charAt(i);
        if (c == ' ' || c == '\t' || c == ':' || c == '-' || c == ',')
            i++;
        else
            break;
    }

    // Remaining is the command string
    int cmdLen = s.length() - i;
    if (cmdLen < 1 || cmdLen > 4)
        return false;

    // Validate command chars: disallow whitespace and comma (since we store as "ID,CMD")
    for (int k = 0; k < cmdLen; k++)
    {
        char c = s.charAt(i + k);
        if (c == ' ' || c == '\t' || c == ',' || c == '\r' || c == '\n')
            return false;
    }

    String cmd = s.substring(i);

    // Produce canonical stored format: "ID,CMD"
    out = String(id) + "," + cmd;
    return true;
}

// === Queue stuff ===

bool CommandParser::dequeueCommand(RingQueue<String, MAX_QUEUE_SIZE> &queue,
                                   String &outCommand)
{
    return queue.dequeue(outCommand);
}

void CommandParser::handleQueueInsertion(
    RingQueue<String, MAX_QUEUE_SIZE> &queue,
    QueueType kind,
    const String &command)
{
    const bool wasFull = queue.isFull();

    // Auto discards if full
    queue.enqueue(command);

    if (wasFull)
    {
        LOGGING(CAT_PARSER, CRIT, "Queue for commands is full; discarded oldest");
        if (kind == QueueType::Radio)
            LOGGING(CAT_PARSER, CRIT, "RADIO queue is full");
        else
            LOGGING(CAT_PARSER, CRIT, "ROCKET queue is full");
    }
}

void CommandParser::enqueueCommand(const String &command)
{
    LOGGING(CAT_PARSER, DEBUG, "Enqueue command triggered");
    if (isRadioCommand(command))
    {
        LOGGING(CAT_PARSER, DEBUG, "Inserting radio command into the radio queue");
        LOGGING(CAT_PARSER, DEBUG, command);
        handleQueueInsertion(radioCommandQueue, QueueType::Radio, command);
    }
    else if (isPingCommand(command))
    {
        // This is only for debugging via serial to send ping to check alive
        LOGGING(CAT_PARSER, DEBUG, "Inserting ping command into the radio queue");
        LOGGING(CAT_PARSER, DEBUG, command);
        handleQueueInsertion(radioCommandQueue, QueueType::Radio, "radio ping");
    }
    else
    {
        LOGGING(CAT_PARSER, DEBUG, "Inserting rocket command into the rocket queue");
        LOGGING(CAT_PARSER, DEBUG, command);

        String normalized;
        if (!normalizeRocketCommand(command, normalized))
        {
            LOGGING(CAT_PARSER, DEBUG, "Rejected rocket command (bad format):");
            LOGGING(CAT_PARSER, DEBUG, command);
            return;
        }

        LOGGING(CAT_PARSER, DEBUG, "Normalized rocket command: ");
        LOGGING(CAT_PARSER, DEBUG, normalized);
        handleQueueInsertion(rocketCommandQueue, QueueType::Rocket, normalized);
    }
}

// === String and char heloers ===
void CommandParser::handleBackspace()
{
    if (currentLen <= 0)
        return;
    // We could also set null terminator, but before we send
    // the command we always null terminate so its ok
    currentLen--;
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
    if (currentLen < PARSER_MAX_COMMAND_LENGTH)
    {
        currentBuf[currentLen++] = c;
        if (LoggerGS::getInstance().getLogLevel() == PIPE)
        {
            Serial.print(c);
        }
    }
    else
    {
        LOGGING(CAT_PARSER, PIPE, F("Warning: Command too long, ignoring extra characters."));
    }
}
