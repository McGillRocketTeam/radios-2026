#include <cstring>

#include "command_packet.h"
#include "frame_printer.h"

#include "ConsoleRouter.h"
#include "GroundStation.h"
#include "LoggerGS.h"
#include "RadioPrinter.h"

// Boolean for command parser ISR
// Tells us if there are things to be processsed in serial
volatile bool GroundStation::commandParserFlag = false;

// === Public Setup of Groundstation ===

GroundStation::GroundStation()
    : radioModule(std::make_unique<RadioModule>()),
      commandParser(std::make_unique<CommandParser>()),
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

GroundStation& GroundStation::getInstance()
{
    static GroundStation instance;
    return instance;
}

void GroundStation::initialise()
{
    startCommandParser();
    startAsyncReceive();
    LOGGING(DEBUG, "Ground station initialised, ready to receive commands via radio and console");
}


// === Private setup helpers ===

void GroundStation::startCommandParser()
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
    // Assuming that we are already in receive mode check if we have an interupt raised
    if (!radioModule->checkInterruptReceived()) return;

    
    // Populate the current Frame View with the new packet
    readReceivedPacket();
    printPacketToGui();
    printVerboseTelemetryPacket();

    // Check the CTS flag and if we want to be able to transmit
    if ( currentFrameView.cts() && canTXFromCTS)
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
    String param;
    String value;

    int firstSpace = radioCommand.indexOf(' ');
    int secondSpace = radioCommand.indexOf(' ', firstSpace + 1);

    if (firstSpace == -1)
    {
        Console.println("Radio command missing a first space");
        return;
    }

    // Extract param and value (if exists)
    if (secondSpace == -1)
    {
        param = radioCommand.substring(firstSpace + 1);
        value = "";
    }
    else
    {
        param = radioCommand.substring(firstSpace + 1, secondSpace);
        value = radioCommand.substring(secondSpace + 1);
    }

    // Now handle all supported commands
    if (param == "freq")
    {
        //"VERIFY THE HW WHEN SETTING THE FREQUENCY"
        radioModule->setFreq(value.toInt());
        currentParams.freq = value.toInt();
        syncCurrentParamsWithRadioModule();
    }
    else if (param == "bw")
    {
        radioModule->setBandwidth(value.toFloat());
        currentParams.bw = value.toFloat();
        syncCurrentParamsWithRadioModule();
        printRadioParamsToGui();
    }
    else if (param == "cr")
    {
        radioModule->setCodingRate(value.toInt());
        currentParams.cr = value.toInt();
        syncCurrentParamsWithRadioModule();
        printRadioParamsToGui();
    }
    else if (param == "sf")
    {
        radioModule->setSpreadingFactor(value.toInt());
        currentParams.sf = value.toInt();
        syncCurrentParamsWithRadioModule();
        printRadioParamsToGui();
    }
    else if (param == "pow")
    {
        radioModule->setPowerOutput(value.toInt());
        currentParams.pow = value.toInt();
        syncCurrentParamsWithRadioModule();
        printRadioParamsToGui();
    }
    else if (param == "param")
    {
        radioModule->checkParams();
    }
    else if (param == "ground")
    {
        radioModule->checkParams();
        Console.print("gsc_verbose_packet: ");
        Console.print(canPrintTelemetryVerbose);
        Console.print(" TxFromCTS ");
        Console.print(canTXFromCTS);
    }
    else if (param == "ping")
    {
        radioModule->pingParams();
    }
    else if (param == "init")
    {
        syncCurrentParamsWithRadioModule();
        printRadioParamsToGui();
    }
    else if (param == "bypass")
    {
        LOGGING(CRIT, "bypass has not been implemented");
    }
    else if (param == "setTx")
    {
        if (value == "t" || value == "f")
        {
            setCanTXFromCTS(value == "t");
            LOGGING(DEBUG, "setting tx from cts");
            LOGGING(DEBUG, value);
        }
        else
        {
            Console.println("setTx accepts only 't' or 'f' as values");
        }
    }
    else if (param == "verbose")
    {
        if (value == "t" || value == "f")
        {
            setVerbosePacket(value == "t");
        }
        else
        {
            Console.println("verbose accepts only 't' or 'f' as values");
        }
    }
    else
    {
        Console.println("command not recognised, please check spelling");
    }
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
        LOGGING(CRIT, "Frequency setting failed");
        return false;
    }
    if (radioModule->getBandwidth() != currentParams.bw)
    {
        LOGGING(CRIT, "Bandwdith setting failed");
        return false;
    }
    if (radioModule->getCodingRate() != currentParams.cr)
    {
        LOGGING(CRIT, "Coding Rate setting failed");
        return false;
    }
    if (radioModule->getSpreadingFactor() != currentParams.sf)
    {
        LOGGING(CRIT, "SF setting failed");
        return false;
    }
    if (radioModule->getPowerOutput() != currentParams.pow)
    {
        LOGGING(CRIT, "Power setting failed");
        return false;
    }
    return true;
}

// === Reading telemetry helpers ===

void GroundStation::readReceivedPacket()
{
    uint8_t *receivedData = radioModule->readPacket();
    int packetLength = radioModule->getPacketLength();
    LOGGING(DEBUG, "Received something time to read it");

    rxLen = min(packetLength, sizeof(rxBuf));
    memcpy(rxBuf, receivedData, rxLen);

    currentFrameView.reset(rxBuf, rxLen);

    if (currentFrameView.validate() != ParseError::Ok) {

        LOGGING( CRIT, "PROBLEMS with frame");
    }
    lastRSSI = radioModule->getRSSI();
    lastSNR = radioModule->getSNR();
}

void GroundStation::printPacketToGui()
{
    Console.sendTelemetry(currentFrameView._base,currentFrameView._len);
    return;
}

void GroundStation::printVerboseTelemetryPacket()
{
    if (!canPrintTelemetryVerbose) return;

    printAtomics( currentFrameView );
    Console.print("Last received RSSI = ");
    Console.print(lastRSSI);
    Console.print(" SNR = ");
    Console.println(lastSNR);
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

void GroundStation::sendRocketCommand(command_packet& command)
{
    if (!radioModule)
    {
        Console.println("Error: Radio module is not initialized.");
        return;
    }
    const size_t length = sizeof(command.data);
    LOGGING(DEBUG, "Sending rocket command serialised length:");
    LOGGING(DEBUG, String(length));

    radioModule->transmitInterrupt((uint8_t*)command.bytes, length);
    radioModule->receiveMode();
}

