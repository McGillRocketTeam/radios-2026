#pragma once

#include "RadioParams.h"
#include "RadioModule.h"
#include "CommandParser.h"
#include "frame_view.h"
#include "IntervalTimer.h"

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

    void printVerboseTelemetryPacket();

    /**
     * @brief Enables or disables TX (transmit) functionality based on CTS logic.
     * @param enable True to enable TX when CTS is set; false to disable.
     */
    void setCanTXFromCTS(bool enable);

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

    void printPacketToGui();

    void readReceivedPacket();

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


    /// Pointer to the radio module
    RadioModule* radioModule;

    /// Pointer to the command parser
    CommandParser* commandParser;

    //Frame view setup 
    // Buffer that persists for the life of Ground Station
    // It will hold the actual bytes for the frame view
    uint8_t rxBuf[512];
    size_t  rxLen = 0;   
    FrameView currentFrameView;

    /// Currently active radio parameters
    RadioParams currentParams;

    /// Timer for raising the commandParserFlag every 10ms
    IntervalTimer commandParserTimer;

    /// Indicates if the system is awaiting an acknowledgment packet
    bool awaitingAck;

    /// Timer for reverting parameters after a timeout
    IntervalTimer paramRevertTimer;

    /// Flag raised by the timer interrupt to indicate it's time to update the parser
    static volatile bool commandParserFlag;

    /// Enables or disables TX based on CTS logic
    bool canTXFromCTS;

    /// Controls if we print the human readble telmetry
    bool canPrintTelemetryVerbose = false;
};

