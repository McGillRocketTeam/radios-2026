#pragma once

#include "RadioParams.h"
#include "RadioModule.h"
#include "CommandParser.h"
#include "PacketController.h"
#include "IntervalTimer.h"
#include <PacketPrinter.h>

/**
 * @class GroundStation
 * @brief Singleton class representing the central logic for the ground station.
 *
 * Manages radio communication, command parsing, packet processing, and parameter handling.
 * Provides methods for asynchronous input handling and packet reception.
 */
class GroundStation {
public:
    /**
     * @brief Get the singleton instance of the GroundStation.
     * @return Pointer to the GroundStation instance.
     */
    static GroundStation* getInstance();

    /**
     * @brief Initialize the ground station subsystems.
     */
    void initialise();

    /**
     * @brief Starts the command parser interrupt which raises a flag every 10ms.
     *
     * This sets up the timer to periodically set a flag. The actual parsing
     * must be handled by calling handleCommandParserUpdate() inside the loop.
     */
    void startCommandParser();

    /**
     * @brief Starts listening for radio packets asynchronously.
     */
    void startAsyncReceive();

    /**
     * @brief Checks the command parser flag and updates the command parser if needed.
     *
     * Should be called repeatedly in the main loop to process user input via Serial.
     */
    void handleCommandParserUpdate();

    /**
     * @brief Grabs the next available radio command and tries to do it.
     */
    void handleRadioCommand();

    /**
     * @brief Processes the most recently received radio packet.
     */
    void handleReceivedPacket();

    /**
     * @brief Handles the execution of the next available rocket command.
     */
    void handleRocketCommand();

    /**
     * @brief Prints queue status (radio/rocket) to Serial.
     */
    void getQueueStatus();

    /**
     * @brief Accessor for the radio module.
     * @return Pointer to the radio module.
     */
    RadioModule* getRadioModule();

    /**
     * @brief Checks with logger if verbose is allowed then prints packet in human readable formate
     * @param packet Pointer to the packet
     */
    template <typename T>
    void printVerboseTelemetryPacket(PacketController *packetController, T *packet);

    /**
     * @brief Enables or disables TX (transmit) functionality based on CTS logic.
     * @param enable True to enable TX when CTS is set; false to disable.
     */
    void setEnableTXFromCTS(bool enable);

    /**
     * @brief Destructor for GroundStation.
     */
    ~GroundStation();

    // Delete copy and move constructors and assignment operators to enforce singleton pattern
    GroundStation(const GroundStation&) = delete;
    GroundStation& operator=(const GroundStation&) = delete;
    GroundStation(GroundStation&&) = delete;
    GroundStation& operator=(GroundStation&&) = delete;

private:
    /**
     * @brief Private constructor for singleton pattern.
     */
    GroundStation();

    /// Singleton instance pointer
    static GroundStation* instance;

    /**
     * @brief Parses and applies rocket command to change radio parameters based on prefix.
     * @param command Full input command.
     * @param prefix Prefix indicating the target parameter set.
     */
    void handleRocketRadioParamChange(String command, String prefix);

    //Get the current radio params of the ground station
    RadioParams getCurrentRadioParams();

    //Make the current radio param match that of the chip
    void matchCurrentRadioParamsWithRadioModule();

    //Check whether the params are matched and if not match them
    void syncCurrentParamsWithRadioModule();

    //Checks current params with whats inside of the radio chip retuns true if its all good
    bool verifyRadioStates();

    //Just like print packet to GUI instead prints the Radio Params held in current Radio Params of GS
    void printRadioParamsToGui();

    /**
     * @brief Applies a new set of radio parameters to the current GS params and sets them in the radio Chip
     * @param rp The new RadioParams object.
     */
    void applyParams(const RadioParams rp);

    /**
     * @brief Reverts the radio parameters to the last known working configuration.
     */
    void revertParams();


    /**
     * @brief Sends a serialised rocket command via the radio module.
     * and puts the radio back to receive mode
     * 
     * @param data pointer to the byte array to send
     * @param length length of the data to send
     */
    void sendSerialisedRocketCommand(const uint8_t *data, size_t length);

    /**
     * @brief Serialises string and sendes the serialised rocket command
     * @param command The command to send.
     */
    void sendRocketCommand(const String& command);

    /**
     * @brief Prints a decoded packet to GUI (if enabled).
     * @param controller The PacketController holding parsed packet data.
     */
    void printPacketToGui(PacketController* controller);

    /**
     * @brief Reads and parses the most recent received packet.
     * @return Pointer to the PacketController containing the packet data.
     */
    PacketController* readReceivedPacket();

    /**
     * @brief Implements parameter changes from a radio command string.
     * @param radioCommand The raw command string from the radio queue.
     */
    void implementRadioParamCommand(String radioCommand);

    /**
     * @brief Static ISR callback to raise the command parser flag.
     */
    static void raiseCommandParserFlag();

    /**
     * @brief Enables or disables verbose packet printing.
     * @param state True to enable verbose printing.
     */
    void setVerbosePacket(bool state);

    /**
     * @brief Enables or disables printing parsed packets to GUI.
     * @param state True to enable GUI output.
     */
    void setPrintToGui(bool state);

    /**
     * @brief Generic method to process and decode any telemetry packet type.
     * @tparam T Type of telemetry packet.
     * @param packet Packet data structure to populate.
     * @param receivedData Raw byte array containing packet data.
     */
    template<typename T>
    void processTelemetryPacket(T &packet, uint8_t *receivedData);

    /**
     * @brief Handles the logic for setting or printing non telemetry info.
     * @param nonTelemetryData the pointer to the raw uint8_t data.
     */
    void handleNonTelemetry(uint8_t * nonTelemetryData);

    /**
     * @brief Checks logger if tabbed booleans is enabled, and then prints the packet's booleans in readable format
     * @param packet pointer to a generic packet type.
     */
    template<typename T>
    void printTabbedBooleans(T *packet);

    template<typename T>
    void printTabbedFCPacket(T *packet);

    /// Pointer to the radio module
    RadioModule* radioModule;

    /// Pointer to the command parser
    CommandParser* commandParser;

    /// Pointer to current packet controller for received packet
    PacketController* currentPacketController;

    /// Currently active radio parameters
    RadioParams currentParams;

    /// Parameters pending application (used for temporary switching)
    RadioParams oldParams;

    /// Timer for raising the commandParserFlag every 10ms
    IntervalTimer commandParserTimer;

    /// Indicates if the system is awaiting an acknowledgment packet
    bool awaitingAck;

    /// Timer for reverting parameters after a timeout
    IntervalTimer paramRevertTimer;

    /// Flag raised by the timer interrupt to indicate it's time to update the parser
    static volatile bool commandParserFlag;

    /// Enables or disables TX based on CTS logic
    bool enableTXFromCTS;

};

