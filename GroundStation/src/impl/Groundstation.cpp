#include <cstring>

#include "command_packet.h"
#include "frame_printer.h"

#include "ConsoleRouter.h"
#include "GSCommand.h"
#include "GroundStation.h"
#include "LoggerGS.h"
#include "RadioMetadata.h"

// Boolean for command parser ISR
// Tells us if there are things to be processsed in serial
volatile bool GroundStation::commandParserFlag = false;

namespace
{
    bool parseBoolean(const String &value, bool &b);
} // anonymous namespace

// === Public Setup of Groundstation ===

GroundStation::GroundStation()
    : radioModule(std::make_unique<RadioModule>()), currentFrameView(), awaitingAck(false), canTXFromCTS(ENABLE_RADIO_TX), canPrintTelemetryVerbose(ENABLE_VERBOSE_TELMETRY_PACKET)
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

void GroundStation::setCanTXFromCTS(bool enable)
{
    canTXFromCTS = enable;
}

void GroundStation::setVerbosePacket(bool state)
{
    canPrintTelemetryVerbose = state;
}

void GroundStation::getQueueStatus()
{
    commandParser->printRadioQueueStatus();
    commandParser->printRocketQueueStatus();
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

void GroundStation::handleRadioCommand()
{
    String radioCommand;
    if (commandParser->getNextRadioCommand(radioCommand))
    {
        implementRadioParamCommand(radioCommand);
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
    if (currentFrameView.cts() && canTXFromCTS)
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

void GroundStation::implementRadioParamCommand(String radioCommand)
{
    String param, value;

    int firstSpace = radioCommand.indexOf(' ');
    int secondSpace = radioCommand.indexOf(' ', firstSpace + 1);

    if (firstSpace == -1)
    {
        Console.println("Radio command missing a first space");
        return;
    }

    if (secondSpace == -1)
    {
        param = radioCommand.substring(firstSpace + 1).toLowerCase();
        value = "";
    }
    else
    {
        param = radioCommand.substring(firstSpace + 1, secondSpace).toLowerCase();
        value = radioCommand.substring(secondSpace + 1);
    }

    const GSCommand::Command cmd = GSCommand::parseRadioCmd(param);

    switch (cmd)
    {
    case GSCommand::Command::Freq:
    {
        const float f = value.toFloat();
        radioModule->setFreq(f);
        break;
    }

    case GSCommand::Command::Bw:
    {
        const float bw = value.toFloat();
        radioModule->setBandwidth(bw);
        break;
    }

    case GSCommand::Command::Cr:
    {
        const int cr = value.toInt();
        radioModule->setCodingRate(cr);
        break;
    }

    case GSCommand::Command::Sf:
    {
        const int sf = value.toInt();
        radioModule->setSpreadingFactor(sf);
        break;
    }

    case GSCommand::Command::Pow:
    {
        const int pow = value.toInt();
        radioModule->setPowerOutput(pow);
        break;
    }

    case GSCommand::Command::Param:
        radioModule->checkParams();
        break;

    case GSCommand::Command::Ground:
        radioModule->checkParams();
        Console.print("gsc_verbose_packet: ");
        Console.print(canPrintTelemetryVerbose);
        Console.print(" TxFromCTS ");
        Console.print(canTXFromCTS);
        break;

    case GSCommand::Command::Ping:
        radioModule->pingParams();
        break;
    // TODO do we need this?
    case GSCommand::Command::Status:
        Console.sendStatus();
        break;

    case GSCommand::Command::Bypass:
        LOGGING(CAT_GS, CRIT, "bypass has not been implemented");
        break;

    case GSCommand::Command::SetTx:
    {
        bool b;
        if (!GSCommand::parseBoolTF(value, b))
        {
            Console.println("setTx accepts only 't' or 'f' as values");
            break;
        }
        setCanTXFromCTS(b);
        LOGGING(CAT_GS, DEBUG, "setting tx from cts");
        LOGGING(CAT_GS, DEBUG, value);
        break;
    }

    case GSCommand::Command::Verbose:
    {
        bool b;
        if (parseBoolean(value, b))
        {
            setVerbosePacket(b);
        }
        break;
    }

    case GSCommand::Command::CatGS:
    case GSCommand::Command::CatParser:
    case GSCommand::Command::CatRadio:
    {
        bool b;
        if (!parseBoolean(value, b))
            break;

        LoggingCategory cat;

        switch (cmd)
        {
        case GSCommand::Command::CatGS:
            cat = CAT_GS;
            break;
        case GSCommand::Command::CatParser:
            cat = CAT_PARSER;
            break;
        case GSCommand::Command::CatRadio:
            cat = CAT_RADIO;
            break;
        default:
            cat = CAT_NONE;
            break;
        }
        LoggerGS::getInstance().setCategory(cat, b);
        break;
    }

    case GSCommand::Command::Unknown:
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

    if (currentFrameState != ParseError::Ok)
    {
        // On bad frame we just log, let send to GUI logic decide what to do
        // with the screwed up frame via currentFrameState
        LOGGING(CAT_GS, CRIT, "Could not validate frame");
        if (currentFrameState == ParseError::PayloadTooShort)
        {
            Serial.println("payload too short");
        }
        if (currentFrameState == ParseError::TooShort)
        {
            Serial.println("header too short");
        }
        if (currentFrameState == ParseError::UnknownAtomicSize)
        {
            Serial.println("uknown atomic");
        }
        return;
    }
    lastRSSI = radioModule->getRSSI();
    lastSNR = radioModule->getSNR();
    lastRawRSSI = radioModule->getRawRSSI();
    lastRawSNR = radioModule->getRawSNR();
}

void GroundStation::sendTelemetryToGui()
{
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

    Console.sendRadioTelemetry(packet.bytes, sizeof(packet.bytes));
    return;
}

void GroundStation::printVerboseTelemetryPacket()
{
    char buf[128] = {0};

    // Standard GS data
    snprintf(buf, sizeof(buf), "RX size=%u seq=%u cts=%u ack=%u bad=%u ack_id=%u gs_t=%.3f gs_rssi=%.2f gs_snr=%.2f", (unsigned)radioModule->getPacketLength(),
             (unsigned)currentFrameView.header()->seq, (unsigned)currentFrameView.cts(), (unsigned)currentFrameView.ack(), (unsigned)currentFrameView.bad(), (unsigned)currentFrameView.ack_id(),
             millis() * 0.001f, lastRSSI, lastSNR);
    LOGGING(CAT_GS, INFO, buf);
    // data for astra debug
    snprintf(buf, sizeof(buf), "ASTRA hdr: seq=%u flags(cts=%u,ack=%u,bad=%u) ack_id=%u", (unsigned)currentFrameView.header()->seq, (unsigned)currentFrameView.cts(), (unsigned)currentFrameView.ack(),
             (unsigned)currentFrameView.bad(), (unsigned)currentFrameView.ack_id());
    LOGGING(CAT_ASTRA_DEBUG, INFO, buf);

    // Special category for logging range test info in csv form
    if (LoggerGS::getInstance().getCategoryMask() & CAT_RANGETEST)
    {
        flight_atomic_data flight{};
        const uint8_t *p = currentFrameView.atomicPtr(AT_FLIGHT_ATOMIC);

        if (p && currentFrameView.hasAtomic(AT_FLIGHT_ATOMIC))
        {
            lastRSSI = radioModule->getRSSI();
            lastSNR = radioModule->getSNR();
            memcpy(&flight, p, sizeof(flight_atomic_data));

            float FC_RSSI = -1.0f * ((int32_t)flight.fc_rssi / 2.0f);
            float FC_SNR = flight.fc_snr * 0.25f;
            float FC_LastTime = flight.gps_time_last_update;
            if (currentFrameView.cts() || currentFrameView.ack())
            {
                const char *kind = currentFrameView.cts() ? "CTS" : "ACK";
                snprintf(buf, sizeof(buf), "%u,%s,GS_T:%.3f,GS_RSSI:%.2f,GS_SNR:%.2f,FC_T:%.3f,FC_RSSI:%.2f,FC_SNR:%.2f", (unsigned)currentFrameView.header()->seq, kind, millis() * 0.001f, lastRSSI,
                         lastSNR, FC_LastTime, FC_RSSI, FC_SNR);
                LOGGING(CAT_RANGETEST, INFO, buf);
            }
        }
    }
    // TODO fix this to work with CATegories instead
    if (canPrintTelemetryVerbose)
    {
        printAtomics(currentFrameView);
    }
    return;
}

// === Sending command helpers ===

void GroundStation::handleRocketCommand()
{
    command_packet rocketCommand;
    rocketCommand.data.command_id = 1;
    std::strncpy(rocketCommand.data.command_string, "nop", sizeof(rocketCommand.data.command_string) - 1);

    commandParser->getNextRocketCommand(rocketCommand);

    sendRocketCommand(rocketCommand);
}

void GroundStation::sendRocketCommand(command_packet &command)
{
    if (!radioModule)
    {
        Console.println("Error: Radio module is not initialized.");
        return;
    }
    const size_t length = sizeof(command.data);

    char buf[128] = {0};

    snprintf(buf, sizeof(buf), "TX cmd=%s id=%u size=%u ", command.data.command_string, (unsigned)command.data.command_id, (unsigned)length);
    LOGGING(CAT_GS, INFO, buf);

    radioModule->transmitBlocking((uint8_t *)command.bytes, length);

    if (strcasecmp(command.data.command_string, "nop") != 0)
    {
        Console.sendCmdAckTx(command.data.command_id);
    }
}

namespace
{

    bool parseBoolean(const String &value, bool &out)
    {
        if (!GSCommand::parseBoolTF(value, out))
        {
            Console.println("verbose accepts only 't' or 'f' as values");
            return false;
        }
        return true;
    }

} // namespace