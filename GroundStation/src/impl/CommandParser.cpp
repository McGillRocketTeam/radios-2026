#include "CommandParser.h"

#include <Arduino.h>
#include <LoggerGS.h>

#include "Config.h"
#include "ConsoleRouter.h"

// === Setup ===

CommandParser& CommandParser::getInstance(){
    static CommandParser inst;
    return inst;
}

CommandParser::CommandParser()
    : radioCommandQueue(QUEUE_SIZE),
      rocketCommandQueue(QUEUE_SIZE),
      currentCommand() {}


// === MAIN public functions ===

void CommandParser::update()
{
    while (Serial.available())
    {
        char receivedChar = Serial.read();

        if (receivedChar == '\n')
        {
            if (currentCommand.length() > 0)
            {
                enqueueCommand(currentCommand);
                currentCommand = "";
            }
        }
        else if (receivedChar == '\b' || receivedChar == 127)
        {
            handleBackspace();
        }
        else if (isPrintableCharacter(receivedChar))
        {
            handleCharacterAppend(receivedChar);
        }
    }
}

bool CommandParser::getNextRadioCommand(String &outCommand)
{
    return dequeueCommand(radioCommandQueue, outCommand);
}

bool CommandParser::getNextRocketCommand(command_packet& outPkt)
{
    String raw;
    if (!dequeueCommand(rocketCommandQueue, raw)) {
        return false;
    }

    int commaIdx = raw.indexOf(',');
    if (commaIdx <= 0 || commaIdx == raw.length() - 1) {
        return false;
    }

    long id = raw.substring(0, commaIdx).toInt();
    if (id < 0 || id > 255) {
        return false;
    }
    outPkt.data.command_id = (uint8_t)id;

    String cmd = raw.substring(commaIdx + 1);

    memset(outPkt.data.command_string, 0, sizeof(outPkt.data.command_string));
    cmd.toCharArray(outPkt.data.command_string,
                    sizeof(outPkt.data.command_string));

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
                                     ArduinoQueue<String> &queue)
{
    Console.print(queueType);
    Console.print(F(" commands in queue: "));
    Console.println(queue.itemCount());
}


// === Private helper functions ===

bool CommandParser::isPingCommand(const String &command)
{
    String trimmed = command;
    trimmed.trim();
    return trimmed == "ping";
}

bool CommandParser::isRadioCommand(const String &command)
{
    int spaceIndex = command.indexOf(' ');
    if (spaceIndex == -1)
        return false;
    return command.substring(0, spaceIndex) == RADIO_COMMAND_KEYWORD;
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

bool CommandParser::dequeueCommand(ArduinoQueue<String> &queue,
                                   String &outCommand)
{
    if (queue.isEmpty())
        return false;
    outCommand = queue.dequeue();
    return true;
}

void CommandParser::handleQueueInsertion(
    ArduinoQueue<String> &queue,
    QueueType kind,
    const String &command)
{
    if (queue.isFull())
    {
        LOGGING(CAT_PARSER,CRIT, "Queue for commands is full discarding old");
        if (kind == QueueType::Radio)
        {
            LOGGING(CAT_PARSER,CRIT, "RADIO queue is full");
        }
        else
        {
            LOGGING(CAT_PARSER,CRIT, "RADIO queue is full");
        }
        queue.dequeue();
    }
    queue.enqueue(command);
}

void CommandParser::enqueueCommand(const String &command)
{
    LOGGING(CAT_PARSER,DEBUG, "Enqueue command triggered");
    if (isRadioCommand(command))
    {
        LOGGING(CAT_PARSER,DEBUG, "Inserting radio command into the radio queue");
        LOGGING(CAT_PARSER,DEBUG, command);
        handleQueueInsertion(radioCommandQueue, QueueType::Radio,command);
    }
    else if (isPingCommand(command))
    {
        // This is only for debugging via serial to send ping to check alive
        LOGGING(CAT_PARSER,DEBUG, "Inserting ping command into the radio queue");
        LOGGING(CAT_PARSER,DEBUG, command);
        handleQueueInsertion(radioCommandQueue, QueueType::Radio, "radio ping");
    }
    else
    {
        LOGGING(CAT_PARSER,DEBUG, "Inserting rocket command into the rocket queue");
        LOGGING(CAT_PARSER,DEBUG, command);

        String normalized;
        if (!normalizeRocketCommand(command, normalized))
        {
            LOGGING(CAT_PARSER,DEBUG, "Rejected rocket command (bad format): " + command);
            return;
        }

        LOGGING(CAT_PARSER,DEBUG, "Normalized rocket command: " + normalized);
        handleQueueInsertion(rocketCommandQueue,QueueType::Rocket,normalized);
    }
}


// === String and char heloers === 
void CommandParser::handleBackspace()
{
    if (currentCommand.length() > 0)
    {
        currentCommand.remove(currentCommand.length() - 1);
        Serial.print("\b \b");
    }
}

bool CommandParser::isPrintableCharacter(char c)
{
    return c >= 32 && c <= 126;
}

void CommandParser::handleCharacterAppend(char c)
{
    if (currentCommand.length() < MAX_COMMAND_LENGTH - 1)
    {
        currentCommand += c;
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
