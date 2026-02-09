#include <cstring>

#include "command_packet.h"
#include "frame_printer.h"

#include "ConsoleRouter.h"
#include "GroundStation.h"
#include "LoggerGS.h"

// Boolean for command parser ISR
// Tells us if there are things to be processsed in serial
volatile bool GroundStation::commandParserFlag = false;

// === Public Setup of Groundstation ===

GroundStation::GroundStation()
    : radioModule(std::make_unique<RadioModule>()),
      currentFrameView(),
      awaitingAck(false),
      canTXFromCTS(ENABLE_RADIO_TX),
      canPrintTelemetryVerbose(ENABLE_VERBOSE_TELMETRY_PACKET)
{
    currentParams.freq = radioModule->getFreq();
    currentParams.bw = radioModule->getBandwidth();
    currentParams.cr = radioModule->getCodingRate();
    currentParams.sf = radioModule->getSpreadingFactor();
    currentParams.pow = radioModule->getPowerOutput();
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
    readReceivedPacket();
    printPacketToGui();
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

    const RadioCmd cmd = parseRadioCmd(param);

    switch (cmd)
    {
    case RadioCmd::Freq:
    {
        const float f = value.toFloat();
        radioModule->setFreq(f);
        currentParams.freq = f;
        syncCurrentParamsWithRadioModule();
        break;
    }

    case RadioCmd::Bw:
    {
        const float bw = value.toFloat();
        radioModule->setBandwidth(bw);
        currentParams.bw = bw;
        syncCurrentParamsWithRadioModule();
        printRadioParamsToGui();
        break;
    }

    case RadioCmd::Cr:
    {
        const int cr = value.toInt();
        radioModule->setCodingRate(cr);
        currentParams.cr = cr;
        syncCurrentParamsWithRadioModule();
        printRadioParamsToGui();
        break;
    }

    case RadioCmd::Sf:
    {
        const int sf = value.toInt();
        radioModule->setSpreadingFactor(sf);
        currentParams.sf = sf;
        syncCurrentParamsWithRadioModule();
        printRadioParamsToGui();
        break;
    }

    case RadioCmd::Pow:
    {
        const int pow = value.toInt();
        radioModule->setPowerOutput(pow);
        currentParams.pow = pow;
        syncCurrentParamsWithRadioModule();
        printRadioParamsToGui();
        break;
    }

    case RadioCmd::Param:
        radioModule->checkParams();
        break;

    case RadioCmd::Ground:
        radioModule->checkParams();
        Console.print("gsc_verbose_packet: ");
        Console.print(canPrintTelemetryVerbose);
        Console.print(" TxFromCTS ");
        Console.print(canTXFromCTS);
        break;

    case RadioCmd::Ping:
        radioModule->pingParams();
        break;

    case RadioCmd::Init:
        syncCurrentParamsWithRadioModule();
        printRadioParamsToGui();
        break;

    case RadioCmd::Bypass:
        LOGGING(CAT_GS, CRIT, "bypass has not been implemented");
        break;

    case RadioCmd::SetTx:
    {
        bool b;
        if (!parseBoolTF(value, b))
        {
            Console.println("setTx accepts only 't' or 'f' as values");
            break;
        }
        setCanTXFromCTS(b);
        LOGGING(CAT_GS, DEBUG, "setting tx from cts");
        LOGGING(CAT_GS, DEBUG, value);
        break;
    }

    case RadioCmd::Verbose:
    {
        bool b;
        if (!parseBoolTF(value, b))
        {
            Console.println("verbose accepts only 't' or 'f' as values");
            break;
        }
        setVerbosePacket(b);
        break;
    }

    case RadioCmd::Unknown:
    default:
        Console.println("command not recognised, please check spelling");
        break;
    }
}

GroundStation::RadioCmd GroundStation::parseRadioCmd(const String &p)
{
    if (p == "freq")
        return RadioCmd::Freq;
    if (p == "bw")
        return RadioCmd::Bw;
    if (p == "cr")
        return RadioCmd::Cr;
    if (p == "sf")
        return RadioCmd::Sf;
    if (p == "pow")
        return RadioCmd::Pow;
    if (p == "param")
        return RadioCmd::Param;
    if (p == "ground")
        return RadioCmd::Ground;
    if (p == "ping")
        return RadioCmd::Ping;
    if (p == "init")
        return RadioCmd::Init;
    if (p == "bypass")
        return RadioCmd::Bypass;
    if (p == "settx")
        return RadioCmd::SetTx;
    if (p == "verbose")
        return RadioCmd::Verbose;
    return RadioCmd::Unknown;
}

bool GroundStation::parseBoolTF(const String &v, bool &out)
{
    if (v == "t")
    {
        out = true;
        return true;
    }
    if (v == "f")
    {
        out = false;
        return true;
    }
    return false;
}

void GroundStation::printRadioParamsToGui()
{
    // TO DO be able to publish all the params to meta data
    return;
}

void GroundStation::syncCurrentParamsWithRadioModule()
{
    if (!verifyRadioStates())
    {
        // State of chip and GS not matching, set the GS current params back to match chip
        currentParams.freq = radioModule->getFreq();
        currentParams.bw = radioModule->getBandwidth();
        currentParams.cr = radioModule->getCodingRate();
        currentParams.sf = radioModule->getSpreadingFactor();
        currentParams.pow = radioModule->getPowerOutput();
    }
}

bool GroundStation::verifyRadioStates()
{
    if (radioModule->getFreq() != currentParams.freq)
    {
        LOGGING(CAT_GS, CRIT, "Frequency setting failed");
        return false;
    }
    if (radioModule->getBandwidth() != currentParams.bw)
    {
        LOGGING(CAT_GS, CRIT, "Bandwdith setting failed");
        return false;
    }
    if (radioModule->getCodingRate() != currentParams.cr)
    {
        LOGGING(CAT_GS, CRIT, "Coding Rate setting failed");
        return false;
    }
    if (radioModule->getSpreadingFactor() != currentParams.sf)
    {
        LOGGING(CAT_GS, CRIT, "SF setting failed");
        return false;
    }
    if (radioModule->getPowerOutput() != currentParams.pow)
    {
        LOGGING(CAT_GS, CRIT, "Power setting failed");
        return false;
    }
    return true;
}

// === Reading telemetry helpers ===

void GroundStation::readReceivedPacket()
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
        LOGGING(CAT_GS, CRIT, "PROBLEMS with frame");
        return;
    }
    lastRSSI = radioModule->getRSSI();
    lastSNR = radioModule->getSNR();
}

void GroundStation::printPacketToGui()
{
    if (currentFrameState != ParseError::Ok)
    {
        Console.println("Cannot print to gui with bad ASTRA frame");
        return;
    }
    Console.sendTelemetry(currentFrameView._base, currentFrameView._len);
    return;
}

void GroundStation::printVerboseTelemetryPacket()
{

    char buf[128] = {0};

    snprintf(buf, sizeof(buf),
             "RX size=%u seq=%u cts=%u ack=%u ack_id=%u gs_t=%.3f gs_rssi=%.2f gs_snr=%.2f",
             (unsigned)radioModule->getPacketLength(),
             (unsigned)currentFrameView.header()->seq,
             (unsigned)currentFrameView.cts(),
             (unsigned)currentFrameView.ack(),
             (unsigned)currentFrameView.ack_id(),
             millis() * 0.001f,
             lastRSSI, lastSNR);
    LOGGING(CAT_GS, CRIT, buf);

    snprintf(buf, sizeof(buf),
             "ASTRA hdr: seq=%u flags(cts=%u,ack=%u) ack_id=%u",
             (unsigned)currentFrameView.header()->seq,
             (unsigned)currentFrameView.cts(),
             (unsigned)currentFrameView.ack(),
             (unsigned)currentFrameView.ack_id());
    LOGGING(CAT_ASTRA_DEBUG, CRIT, buf);

    if (LoggerGS::getInstance().getCategoryMask() & CAT_RANGETEST)
    {
        flight_atomic_data flight{};
        const uint8_t *p = currentFrameView.atomicPtr(AT_FLIGHT_ATOMIC);

        if (p && currentFrameView.hasAtomic(AT_FLIGHT_ATOMIC))
        {
            lastRSSI = radioModule->getRSSI();
            lastSNR = radioModule->getSNR();
            memcpy(&flight, p, sizeof(flight_atomic_data));

            float FC_RSSI = ((int32_t)flight.fc_rssi - 400) * 0.5f;
            float FC_SNR = flight.fc_snr * 0.25f;
            float FC_LastTime = flight.gps_time_last_update;
            if (currentFrameView.cts() || currentFrameView.ack())
            {
                const char *kind = currentFrameView.cts() ? "CTS" : "ACK";
                snprintf(buf, sizeof(buf),
                         "%u,%s,GS_T:%.3f,GS_RSSI:%.2f,GS_SNR:%.2f,FC_T:%.3f,FC_RSSI:%.2f,FC_SNR:%.2f",
                         (unsigned)currentFrameView.header()->seq,
                         kind,
                         millis() * 0.001f, lastRSSI, lastSNR,
                         FC_LastTime, FC_RSSI, FC_SNR);
                LOGGING(CAT_RANGETEST, CRIT, buf);
            }
        }
    }

    if (!canPrintTelemetryVerbose)
        return;

    // Console.print("SIZE: ");
    // Console.print(radioModule->getPacketLength());
    // Console.print(" CTS: ");
    // Console.print(currentFrameView.cts());
    // Console.print(" ACK: ");
    // Console.print(currentFrameView.ack());
    // Console.print(" ACK_ID: ");
    // Console.println(currentFrameView.ack_id());

    // {
    //     const uint8_t* p = (const uint8_t*)(currentFrameView.header());
    //     for (size_t i = 0; i < sizeof(FrameHeader); i++)
    //     {
    //         if (p[i] < 16) Serial.print('0');
    //         Serial.print(p[i], HEX);
    //         if (i + 1 < sizeof(FrameHeader)) Serial.print(' ');
    //     }
    //     Serial.println();
    // }
}

// === Sending command helpers ===

void GroundStation::handleRocketCommand()
{
    command_packet rocketCommand;
    rocketCommand.data.command_id = 1;
    std::strncpy(rocketCommand.data.command_string, "nop",
                 sizeof(rocketCommand.data.command_string) - 1);

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
    LOGGING(CAT_GS, DEBUG, "Sending rocket command serialised length:");
    LOGGING(CAT_GS, DEBUG, String(length));

    radioModule->transmitBlocking((uint8_t *)command.bytes, length);

    if (strcasecmp(command.data.command_string, "nop") != 0)
    {
        Console.sendCmdAckTx(command.data.command_id);
    }
}
