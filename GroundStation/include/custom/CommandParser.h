#pragma once

/**
 * @file CommandParser.h
 * @brief Serial-based command parser for ground station interface.
 *
 * Captures and parses user-typed commands character by character asynchronously,
 * differentiates "radio" vs "rocket" commands using keywords, and queues the commands
 * into separate queues.
 */

#include <Arduino.h>
#include <ArduinoQueue.h>
#include "Config.h"

/// Maximum length (in chars) for any single command line.
#define MAX_COMMAND_LENGTH 100

/// Maximum number of commands retained per queue.
#define QUEUE_SIZE 50

/**
 * @class CommandParser
 * @brief Parses incoming Serial input into categorized command queues.
 *
 * Processes serial input from the user, supporting real-time backspace editing.
 * On receiving a newline, it determines whether the command is radio-related or not
 * and enqueues it in the corresponding queue. Older entries are discarded when queue is full.
 */
class CommandParser {
public:
    /**
     * @brief Construct a new CommandParser object.
     */
    CommandParser();

    /**
     * @brief Reads Serial input and updates the parser state.
     *
     * This function should be called continuously (e.g., inside the main loop)
     * to consume incoming characters from Serial. It supports backspace editing
     * and enqueues complete commands upon newline.
     */
    void update();

    /**
     * @brief Retrieves the next radio command, if available.
     * @param[out] outCommand String object populated with the dequeued command.
     * @return true if a command was available and dequeued.
     * @return false if the radio command queue was empty.
     */
    bool getNextRadioCommand(String &outCommand);

    /**
     * @brief Retrieves the next rocket command, if available.
     * @param[out] outCommand String object populated with the dequeued command.
     * @return true if a command was available and dequeued.
     * @return false if the rocket command queue was empty.
     */
    bool getNextRocketCommand(String &outCommand);

    /**
     * @brief Prints the number of pending radio commands to Serial.
     */
    void printRadioQueueStatus();

    /**
     * @brief Prints the number of pending rocket commands to Serial.
     */
    void printRocketQueueStatus();

private:
    ArduinoQueue<String> radioCommandQueue;   ///< Queue for radio commands.
    ArduinoQueue<String> rocketCommandQueue;  ///< Queue for rocket (non-radio) commands.
    String currentCommand;                    ///< Buffer for building the current command.

    /**
     * @brief Enqueues the given command into the appropriate queue.
     * @param command The command string to enqueue.
     */
    void enqueueCommand(String &command);

    /**
     * @brief Determines whether the given command is a radio command.
     * @param command The command string to evaluate.
     * @return true if it is a radio command (based on keyword); false otherwise.
     */
    bool isRadioCommand(const String &command);

    bool isPingCommand(const String &command);

    

    /**
     * @brief Inserts a command into a specified queue and logs overflow if necessary.
     * @param queue The target queue.
     * @param command The command string to insert.
     * @param queueType A string identifier (e.g., "radio" or "rocket") used for logging/debug.
     */
    void handleQueueInsertion(ArduinoQueue<String> &queue,
                              const String &command,
                              const char *queueType);

    /**
     * @brief Handles backspace character by removing the last character from the buffer.
     */
    void handleBackspace();

    /**
     * @brief Checks whether a character is printable (excluding control characters).
     * @param c The character to check.
     * @return true if printable; false otherwise.
     */
    bool isPrintableCharacter(char c);

    /**
     * @brief Appends a character to the current command buffer.
     * @param c The character to append.
     */
    void handleCharacterAppend(char c);

    /**
     * @brief Attempts to dequeue a command from the specified queue.
     * @param queue The source queue.
     * @param[out] outCommand The string to populate with the dequeued command.
     * @return true if a command was successfully dequeued; false otherwise.
     */
    bool dequeueCommand(ArduinoQueue<String> &queue, String &outCommand);

    /**
     * @brief Prints the size of a given queue to Serial.
     * @param queueType A descriptive name (e.g., "radio", "rocket") for logging.
     * @param queue The queue whose size will be printed.
     */
    void printQueueStatus(const char *queueType,
                          ArduinoQueue<String> &queue);

};

