#include <cmath>
#include <cstring>

#include "GroundStation.h"

#include "command_packet.h"
#include "frame_printer.h"

#include "ConsoleRouter.h"
#include "GroundCommand.h"
#include "GroundStationStore.h"
#include "LoggerGS.h"
#include "RocketCommand.h"
#include "RadioMetadata.h"

// Boolean for command parser ISR
// Tells us if there are things to be processsed in serial
volatile bool GroundStation::commandParserFlag = false;

namespace
{
    constexpr int16_t INVALID_JUNCTION_TEMP_CENTI_C = INT16_MIN;

    int16_t getJunctionTempCentiC()
    {
#if defined(__IMXRT1062__)
        const float tempC = tempmonGetTemp();
        if (!isfinite(tempC))
        {
            return INVALID_JUNCTION_TEMP_CENTI_C;
        }

        float centiC = roundf(tempC * 100.0f);
        if (centiC < static_cast<float>(INT16_MIN + 1))
        {
            centiC = static_cast<float>(INT16_MIN + 1);
        }
        if (centiC > static_cast<float>(INT16_MAX))
        {
            centiC = static_cast<float>(INT16_MAX);
        }
        return static_cast<int16_t>(centiC);
#else
        return INVALID_JUNCTION_TEMP_CENTI_C;
#endif
    }

    bool parseBoolean(const float value)
    {
        if (value == 0)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
} // anonymous namespace

// === Public Setup of Groundstation ===

GroundStation::GroundStation()
    : radioModule(std::make_unique<RadioModule>()), currentFrameView()
{
}

GroundStation &GroundStation::getInstance()
{
    static GroundStation instance;
    return instance;
}

void GroundStation::initialise(CommandParser &parser)
{
    commandParser = &parser;
    startCommandParserSerial();
    startAsyncReceive();
    LOGGING(CAT_GS, DEBUG, "Ground station initialised, ready to receive commands via radio and console");
}

// === Private setup helpers ===

void GroundStation::startCommandParserSerial()
{
    commandParserTimer.begin(raiseCommandParserFlag, 10000);
}

void GroundStation::startAsyncReceive()
{
    radioModule->receiveMode();
}

// === Getters and Setters ===
RadioModule *GroundStation::getRadioModule()
{
    return radioModule.get();
}


void GroundStation::setVerbosePacket(bool state)
{
    LoggerGS::getInstance().setCategory(CAT_TELEMETRY, state);
}

void GroundStation::clearRocketCommandQueue()
{
    commandParser->clearRocketQueue();
    Console.sendRadioCmdAck();
}

void GroundStation::getQueueStatus()
{
    commandParser->printGroundQueueStatus();
    commandParser->printRocketQueueStatus();
}

void GroundStation::setPrintHook(PrintHook hook)
{
    verbosePrintHook = hook;
}

// === MAIN public functions ===

void GroundStation::handleCommandParserUpdate()
{
    if (!commandParser)
    {
        LOGGING(CAT_GS, CRIT, "Command parser not initialised in the GS!");
        return;
    }
    if (commandParserFlag)
    {
        commandParserFlag = false;
        commandParser->update();
    }
}

void GroundStation::handleGroundCommand()
{
    GroundCommand::Cmd groundCommand;
    if (commandParser->getNextGroundCommand(groundCommand))
    {
        implementGroundCommand(groundCommand);
    }
}

void GroundStation::handleReceivedPacket()
{
    // Check if we got an interupt and if it has led to a valid packet
    if (!radioModule->pollValidPacketRx())
        return;
    // Populate the current Frame View with the new packet
    readReceivedPacketToFrame();
    sendTelemetryToGui();
    printVerboseTelemetryPacket();

    // Check the CTS flag and if we want to be able to transmit
    if (currentFrameView.cts() && GroundStationStore::canTxFromCTS())
    {
        // With the CTS flag we know to send another command to the rocket,
        handleRocketCommand();
    }
}

// === Command Parsing from Serial ===

void GroundStation::raiseCommandParserFlag()
{
    commandParserFlag = true;
}

// === Radio Command helpers ===

void GroundStation::implementGroundCommand(GroundCommand::Cmd command)
{

    // Switch case on the possible actions
    switch (command.action)
    {
    case GroundCommand::Action::Freq:
    {
        radioModule->setFreq(command.arg);
        Console.sendStatusOk();
        break;
    }

    case GroundCommand::Action::Bw:
    {
        radioModule->setBandwidth(command.arg);
        Console.sendStatusOk();
        break;
    }

    case GroundCommand::Action::Cr:
    {
        uint8_t cr = (uint8_t)floorf(command.arg);
        radioModule->setCodingRate(cr);
        Console.sendStatusOk();
        break;
    }

    case GroundCommand::Action::Sf:
    {
        uint8_t sf = (uint8_t)floorf(command.arg);
        radioModule->setSpreadingFactor(sf);
        Console.sendStatusOk();
        break;
    }

    case GroundCommand::Action::Pow:
    {
        int8_t pow = (int8_t)floorf(command.arg);
        radioModule->setPowerOutput(pow);
        Console.sendStatusOk();
        break;
    }

    case GroundCommand::Action::Param:
        radioModule->checkParams();
        break;

    case GroundCommand::Action::Ground:
        radioModule->checkParams();
        Console.print(" TxFromCTS ");
        Console.print(GroundStationStore::canTxFromCTS());
        break;

    case GroundCommand::Action::Ping:
        radioModule->pingParams();
        break;

    case GroundCommand::Action::Bypass:
        LOGGING(CAT_GS, CRIT, "bypass has not been implemented");
        break;

    case GroundCommand::Action::SetStatus:
    {
        bool b = parseBoolean(command.arg);
        if (b)
            Console.sendStatusOk();
        else
            Console.sendStatusFailed();
        break;
    }

    case GroundCommand::Action::SetTx:
    {
        bool b = parseBoolean(command.arg);
        GroundStationStore::setCanTxFromCTS(b);
        char buf[64];
        snprintf(buf, sizeof(buf), "setting tx from cts b=%s", b ? "true" : "false");
        LOGGING(CAT_GS, DEBUG, buf);
        // Support the GSC setting the txFromCTS policy
        Console.sendRadioCmdAck();
        // Refresh the status as well since TxFromCTS is a ground config
        Console.sendStatusOk();
        break;
    }

    case GroundCommand::Action::Verbose:
    {
        setVerbosePacket(parseBoolean(command.arg));
        break;
    }

    case GroundCommand::Action::Clear:
    {
        clearRocketCommandQueue();
        break;
    }

    case GroundCommand::Action::CatGS:
    case GroundCommand::Action::CatParser:
    case GroundCommand::Action::CatRadio:
    {
        bool b = parseBoolean(command.arg);

        LoggingCategory cat;

        switch (command.action)
        {
        case GroundCommand::Action::CatGS:
            cat = CAT_GS;
            break;
        case GroundCommand::Action::CatParser:
            cat = CAT_PARSER;
            break;
        case GroundCommand::Action::CatRadio:
            cat = CAT_RADIO;
            break;
        default:
            cat = CAT_NONE;
            break;
        }
        LoggerGS::getInstance().setCategory(cat, b);
        break;
    }

    case GroundCommand::Action::Unknown:
    default:
        Console.println("command not recognised, please check spelling");
        break;
    }
}

// === Reading telemetry helpers ===

void GroundStation::readReceivedPacketToFrame()
{
    uint8_t *receivedData = radioModule->getPacketData();
    int packetLength = radioModule->getPacketLength();
    LOGGING(CAT_GS, DEBUG, "Received something time to read it");

    rxLen = min(packetLength, sizeof(rxBuf));
    memcpy(rxBuf, receivedData, rxLen);

    currentFrameView.reset(rxBuf, rxLen);
    currentFrameState = currentFrameView.validate();

    switch (currentFrameState)
    {
    case ParseError::Ok:
        // No error save metadata and continue
        lastRSSI = radioModule->getRSSI();
        lastSNR = radioModule->getSNR();
        lastRawRSSI = radioModule->getRawRSSI();
        lastRawSNR = radioModule->getRawSNR();
        break;
    case ParseError::COMMAND_PACKET:
    case ParseError::COMMAND_PACKET_EXTENDED:
        // TODO perhaps logging the cmd packet info more aggresively
        // is worth it? Needs evaluating
        LOGGING(CAT_GS, INFO, "command intercept");
        break;
    case ParseError::PayloadTooShort:
        LOGGING(CAT_GS, CRIT, "payload too short");
        break;
    case ParseError::TooShort:
        LOGGING(CAT_GS, CRIT, "header too short");
        break;
    case ParseError::UnknownAtomicSize:
        LOGGING(CAT_GS, CRIT, "Unknown atomic size");
        break;
    }
}

void GroundStation::sendTelemetryToGui()
{
    if (currentFrameState == ParseError::COMMAND_PACKET ||
        currentFrameState == ParseError::COMMAND_PACKET_EXTENDED)
    {
        // On command intercepts we dont send anything to the GUI
        return;
    }
    // TODO discuss how to alert GUI to bad ASTRA frames
    if (currentFrameState != ParseError::Ok)
    {
        LOGGING(CAT_GS, CRIT, "Force Sending BAD ASTRA frame to GUI");
        Console.sendTelemetry(currentFrameView._base, currentFrameView._len);
        return;
    }

    Console.sendTelemetry(currentFrameView._base, currentFrameView._len);

    // Construct and send associated radio telemetry to GUI
    radio_metadata_data_packet packet = {0};
    packet.data.radio_rssi = lastRawRSSI;
    packet.data.radio_snr = lastRawSNR;
    packet.data.seq = currentFrameView.header()->seq;
    packet.data.junction_temp_centi_c = getJunctionTempCentiC();

    Console.sendRadioTelemetry(packet.bytes, sizeof(packet.bytes));
    return;
}

void GroundStation::printVerboseTelemetryPacket()
{
    // If we have set a special verbose printHook differ to that
    if (verbosePrintHook)
    {
        verbosePrintHook(currentFrameView, currentFrameState, lastRSSI, lastSNR);
        return;
    }
    if (currentFrameState != ParseError::Ok)
        return;

    char buf[128] = {0};

    // Standard GS data
    snprintf(buf, sizeof(buf),
             "RX seq=%u cts=%u ack=%u bad=%u nak=%u ack_id=%u gs_rssi=%.2f gs_snr=%.2f len=%u gs_t=%.3f",
             (unsigned)currentFrameView.header()->seq,
             (unsigned)currentFrameView.cts(),
             (unsigned)currentFrameView.ack(),
             (unsigned)currentFrameView.bad(),
             (unsigned)currentFrameView.nak(),
             (unsigned)currentFrameView.ack_id(),
             lastRSSI,
             lastSNR,
             (unsigned)currentFrameView._len,
             millis() * 0.001f);
    LOGGING(CAT_GS, INFO, buf);

    if (LoggerGS::getInstance().getCategoryMask() & CAT_TELEMETRY)
    {
        printAtomics(currentFrameView);
    }
}

// === Sending command helpers ===

void GroundStation::handleRocketCommand()
{
    command_packet_extended rocketCommand = {0};
    // Populate with default nop
    rocketCommand.data.base.data.command_id = 1;
    std::strncpy(
        rocketCommand.data.base.data.command_string,
        "nop",
        sizeof(rocketCommand.data.base.data.command_string) - 1);

    commandParser->getNextRocketCommand(rocketCommand);

    sendRocketCommand(rocketCommand);
}

void GroundStation::sendRocketCommand(command_packet_extended &command)
{
    if (!radioModule)
    {
        Console.println("Error: Radio module is not initialized.");
        return;
    }
    // CRITICAL, here we decide to send just the header or the whole extended

    char buf[128] = {0};
    size_t length;
    if (command.data.argc == 0)
    {
        // Sending the shortedned header only
        length = sizeof(command.data.base.bytes);
        radioModule->transmitBlocking((uint8_t *)command.data.base.bytes, length);
    }
    else
    {
        // Sending the full extended
        length = sizeof(command.bytes);
        radioModule->transmitBlocking((uint8_t *)command.bytes, length);
    }

    snprintf(buf, sizeof(buf), "TX cmd=%s id=%u size=%u ",
             command.data.base.data.command_string,
             (unsigned)command.data.base.data.command_id,
             (unsigned)length);

    LOGGING(CAT_GS, INFO, buf);

    if (strcasecmp(command.data.base.data.command_string, "nop") != 0)
    {
        Console.sendCmdAckTx(command.data.base.data.command_id, true);
    }
}
