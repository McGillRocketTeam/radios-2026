#pragma once

#include <Arduino.h>
#include <cstdint>

#include "Config.h"
#include "CommandLine.h"
#include "RingQueue.h"
#include "command_packet.h"
#include "GroundCommand.h"

/// Maximum number of commands retained per queue.
constexpr uint16_t MAX_QUEUE_SIZE = 30;

/**
 * @class CommandParser
 * @brief Parses incoming Serial input into categorized command queues.
 *
 * Processes serial input from the user, supporting real-time backspace editing.
 * On receiving a newline, it determines whether the command is radio-related or not
 * and enqueues it in the corresponding queue. Older entries are discarded when queue is full.
 */
class CommandParser
{
public:
    static CommandParser &getInstance();

    // Main call to check serial for inputs
    void update();

    bool getNextGroundCommand(GroundCommand::Cmd &out);

    // Loads command into out, if false means no command, and we dont touch out
    bool getNextRocketCommand(command_packet_extended &outCommand);

    void printGroundQueueStatus();

    void printRocketQueueStatus();

    // Always queues to command, discards oldest if full,
    // returns false if we cant parse the command_line
    bool enqueueCommand(const command_line &line);

    bool isRocketQueueFull();

private:
    RingQueue<GroundCommand::Cmd, MAX_QUEUE_SIZE> groundCommandQueue;
    RingQueue<command_packet_extended, MAX_QUEUE_SIZE> rocketCommandQueue;

    // Buffer for building the current command.
    command_line currentLine;

    CommandParser();

    void handleBackspace();

    bool isPrintableCharacter(char c);

    void handleCharacterAppend(char c);
};
