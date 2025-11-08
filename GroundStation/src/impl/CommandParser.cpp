// // src/CommandParser.cpp
// #include "CommandParser.h"
// #include <Arduino.h>
// #include "Config.h"
// #include <LoggerGS.h>
// #include <ConsoleRouter.h>

// CommandParser::CommandParser()
//     : radioCommandQueue(QUEUE_SIZE),
//       rocketCommandQueue(QUEUE_SIZE),
//       currentCommand() {}

// void CommandParser::update() {
//     while (Console.available()) {
//         char receivedChar = Console.read();

//         if (receivedChar == '\n') {
//             if (currentCommand.length() > 0) {
//                 enqueueCommand(currentCommand);
//                 if (LoggerGS::getInstance().isEnabled(DEBUG_PARSER)){
//                     Console.println();
//                 }
//                 currentCommand = "";
//             }
//         } else if (receivedChar == '\b' || receivedChar == 127) {
//             handleBackspace();
//         } else if (isPrintableCharacter(receivedChar)) {
//             handleCharacterAppend(receivedChar);
//         }
//     }
// }

// bool CommandParser::getNextRadioCommand(String &outCommand) {
//     return dequeueCommand(radioCommandQueue, outCommand);
// }

// bool CommandParser::getNextRocketCommand(String &outCommand) {
//     return dequeueCommand(rocketCommandQueue, outCommand);
// }

// void CommandParser::printRadioQueueStatus() {
//     printQueueStatus("Radio", radioCommandQueue);
// }

// void CommandParser::printRocketQueueStatus() {
//     printQueueStatus("Rocket", rocketCommandQueue);
// }

// void CommandParser::enqueueCommand(String &command) {
//     LOGGING(DEBUG_PARSER,"Enqueue command triggered");
//     if (isRadioCommand(command)) {
//         LOGGING(DEBUG_PARSER,"Inserting radio command into the radio queue");
//         LOGGING(DEBUG_PARSER,command);
//         handleQueueInsertion(radioCommandQueue, command, "Radio");
//     } else if (isPingCommand(command)) {
//         LOGGING(DEBUG_PARSER,"Inserting ping command into the radio queue");
//         LOGGING(DEBUG_PARSER,command);
//         handleQueueInsertion(radioCommandQueue, "radio ping", "Radio");
//     }
//     else {
//         LOGGING(DEBUG_PARSER,"Inserting rocket command into the rocket queue");
//         LOGGING(DEBUG_PARSER,command);
//         handleQueueInsertion(rocketCommandQueue, command, "Rocket");
//     }
// }

// bool CommandParser::isPingCommand(const String &command) {
//     String trimmed = command;
//     //Remove the leading and trailing white space for ping
//     trimmed.trim();  
//     return trimmed == "ping";
// }

// bool CommandParser::isRadioCommand(const String &command) {
//     int spaceIndex = command.indexOf(' ');
//     if (spaceIndex == -1) return false;
//     return command.substring(0, spaceIndex) == RADIO_COMMAND_KEYWORD;
// }

// void CommandParser::handleQueueInsertion(ArduinoQueue<String> &queue,
//                                          const String &command,
//                                          const char *queueType) {
//     if (queue.isFull()) {
//         Console.print(F("Warning: "));
//         Console.print(queueType);
//         Console.println(F(" Command queue full, discarding old command."));
//         queue.dequeue();
//     }
//     queue.enqueue(command);
// }

// void CommandParser::handleBackspace() {
//     if (currentCommand.length() > 0) {
//         currentCommand.remove(currentCommand.length() - 1);
//         Console.print("\b \b");
//     }
// }

// bool CommandParser::isPrintableCharacter(char c) {
//     return c >= 32 && c <= 126;
// }

// void CommandParser::handleCharacterAppend(char c) {
//     if (currentCommand.length() < MAX_COMMAND_LENGTH - 1) {
//         currentCommand += c;
//         if (LoggerGS::getInstance().isEnabled(DEBUG_PARSER)){
//             Console.print(c);
//         }
//     } else {
//         Console.println(F("Warning: Command too long, ignoring extra characters."));
//     }
// }

// bool CommandParser::dequeueCommand(ArduinoQueue<String> &queue,
//                                    String &outCommand) {
//     if (queue.isEmpty()) return false;
//     outCommand = queue.dequeue();
//     return true;
// }

// void CommandParser::printQueueStatus(const char *queueType,
//                                      ArduinoQueue<String> &queue) {
//     Console.print(queueType);
//     Console.print(F(" commands in queue: "));
//     Console.println(queue.itemCount());
// }
