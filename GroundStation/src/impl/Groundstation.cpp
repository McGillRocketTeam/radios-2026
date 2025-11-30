#include "GroundStation.h"
#include <ConsoleRouter.h>
#include "frame_printer.h"
#include "RadioPrinter.h"
#include <LoggerGS.h>
#include "command_packet.h"

volatile bool GroundStation::commandParserFlag = false;

GroundStation::GroundStation()
    : radioModule(new RadioModule()),
      commandParser(new CommandParser()),
      currentFrameView(),
      awaitingAck(false),
      canTXFromCTS(ENABLE_RADIO_TX)
{
    currentParams.freq = radioModule->getFreq();
    currentParams.bw = radioModule->getBandwidth();
    currentParams.cr = radioModule->getCodingRate();
    currentParams.sf = radioModule->getSpreadingFactor();
    currentParams.pow = radioModule->getPowerOutput();
}

GroundStation::~GroundStation()
{
    delete radioModule;
    delete commandParser;
}

GroundStation *GroundStation::getInstance()
{
    if (instance == nullptr)
    {
        instance = new GroundStation();
    }
    return instance;
}

void GroundStation::initialise()
{
    startCommandParser();
    startAsyncReceive();
    LOGGING(DEBUG, "Ground station initialised, ready to receive commands via radio and console");
}

RadioModule *GroundStation::getRadioModule()
{
    return radioModule;
}

void GroundStation::startCommandParser()
{
    commandParserTimer.begin(raiseCommandParserFlag, 10000);
}

void GroundStation::startAsyncReceive()
{
    radioModule->receiveMode();
}

RadioParams GroundStation::getCurrentRadioParams()
{
    return currentParams;
}

void GroundStation::matchCurrentRadioParamsWithRadioModule()
{
    currentParams.freq = radioModule->getFreq();
    currentParams.bw = radioModule->getBandwidth();
    currentParams.cr = radioModule->getCodingRate();
    currentParams.sf = radioModule->getSpreadingFactor();
    currentParams.pow = radioModule->getPowerOutput();
}

void GroundStation::printPacketToGui()
{
    Console.sendTelemetry(currentFrameView._base,currentFrameView._len);
    return;
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
    if (radioModule->checkInterruptReceived())
    {
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
}

void GroundStation::handleRocketCommand()
{
    String rocketCommand = "nop";
    commandParser->getNextRocketCommand(rocketCommand);

    // Check for our magic “change param” command:
    const String prefix = "change param ";
    if (rocketCommand.startsWith(prefix))
    {
        handleRocketRadioParamChange(rocketCommand, prefix);
    }
    else
    {
        // Otherwise normal send:
        sendRocketCommand(rocketCommand);
    }
}

void GroundStation::handleRocketRadioParamChange(String rocketRadioParamChangeCommand, String rocketRadioChangePrefix)
{
    // Grab the part after the prefix
    String nums = rocketRadioParamChangeCommand.substring(rocketRadioChangePrefix.length());
    // e.g. "903,7,125,8"
    // change param 433 8 125 8 (freq,sf,bw,cr)

    // Split on spaces
    float parts[4] = {0};
    int idx = 0;
    int start = 0;
    while (idx < 4)
    {
        int comma = nums.indexOf(' ', start);
        String token;
        if (comma == -1)
        {
            token = nums.substring(start);
        }
        else
        {
            token = nums.substring(start, comma);
        }
        parts[idx++] = token.toFloat();
        if (comma == -1)
            break;
        start = comma + 1;
    }
    if (idx != 4)
    {
        LOGGING(DEBUG, "Invalid change param format; expect four space separated values");
        return;
    }

    // Save old, set pending (leave power unchanged)
    oldParams = currentParams;

    currentParams.freq = (parts[0]);
    currentParams.sf = static_cast<int>(parts[1]);
    currentParams.bw = (parts[2]);
    currentParams.cr = static_cast<int>(parts[3]);

    // New Lora Params (FREQ=W (uint32_t), SF=X (uint8_t), BW=Y (uint8_t), CR=Z (uint8_t)
    uint8_t buf[9];
    buf[0] = 'r';
    buf[1] = 'l';
    buf[2] = static_cast<uint8_t>(parts[1]); // SF
    buf[3] = toLoraBwEnum(parts[2]); // Convert BW from GS MHz format to rocket enum format

    uint32_t freq_hz = static_cast<uint32_t>(parts[0] * 1e6); // Convert MHz to Hz
    memcpy(&buf[4], &freq_hz, 4);                             // Little endian assumed

    buf[8] = toLoraCrEnum(parts[3]); // Convert CR from GS format to rocket enum format

    sendSerialisedRocketCommand(buf, 9);

    LOGGING(INFO, "Sending a ROCKET RADIO PARAM CHANGE command");

    applyParams(currentParams);

    // Tell the GUI of the theoretical new params if we fail we resend in revertParams
    printRadioParamsToGui();
    // Arm the revert timer:
    awaitingAck = true;
    paramRevertTimer.begin([]()
                           { GroundStation::getInstance()->revertParams(); }, 10000000); // 10 000 000 µs == 5 s
    return;
}

void GroundStation::getQueueStatus()
{
    commandParser->printRadioQueueStatus();
    commandParser->printRocketQueueStatus();
}

void GroundStation::setCanTXFromCTS(bool enable)
{
    canTXFromCTS = enable;
}

void GroundStation::raiseCommandParserFlag()
{
    commandParserFlag = true;
}

void GroundStation::handleCommandParserUpdate()
{
    if (commandParserFlag)
    {
        commandParserFlag = false;
        commandParser->update();
    }
}

void GroundStation::applyParams(const RadioParams rp)
{
    radioModule->setFreq(rp.freq);
    radioModule->setBandwidth(rp.bw);
    radioModule->setCodingRate(rp.cr);
    radioModule->setSpreadingFactor(rp.sf);
    radioModule->setPowerOutput(rp.pow);

    currentParams.freq = rp.freq;
    currentParams.bw = rp.bw;
    currentParams.cr = rp.cr;
    currentParams.sf = rp.sf;
    currentParams.pow = rp.pow;
}

void GroundStation::revertParams()
{
    if (!awaitingAck)
        return;
    applyParams(oldParams);
    // Update GUI since we need to revert
    printRadioParamsToGui();
    awaitingAck = false;
    LOGGING(INFO, "No packets on new params; reverted to old settings.");
}

void GroundStation::setVerbosePacket(bool state)
{
    canPrintTelemetryVerbose = state;
}


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
        sendRocketCommand(value);
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

void GroundStation::sendSerialisedRocketCommand(const uint8_t *data, size_t length)
{
    if (!radioModule)
    {
        Console.println("Error: Radio module is not initialized.");
        return;
    }

    LOGGING(DEBUG, "Sending rocket command serialised length:" + String(length));
    //Always send at least once the transmit 
    radioModule->transmitInterrupt(data, length);
    radioModule->receiveMode();
}

void GroundStation::sendRocketCommand(const String &rocketCommand)
{
    // String formattedCommand = String(RADIO_CALL_SIGN) + " " + rocketCommand;
    String formattedCommand;
    if (rocketCommand.length() == 3) 
    {
        command_packet packet;

        packet.data.command_id = static_cast<uint8_t>(rocketCommand.charAt(0) - '0');  // Convert '3' to 3

        packet.data.command_string[0] = rocketCommand.charAt(1);  
        packet.data.command_string[1] = rocketCommand.charAt(2);

        packet.data.command_string[2] = '\0';
        packet.bytes[sizeof(packet.data) - 1] = '\0';

        formattedCommand = String(reinterpret_cast<char*>(packet.bytes));
    }
    else
    {
        formattedCommand = rocketCommand;
    }
    LOGGING(DEBUG, "Sending to rocket string command:" + formattedCommand);

    sendSerialisedRocketCommand(reinterpret_cast<const uint8_t *>(formattedCommand.c_str()),
                                formattedCommand.length() + 1); // Sending an extra for the null terminator
}


void GroundStation::readReceivedPacket()
{
    uint8_t *receivedData = radioModule->readPacket();
    int packetLength = radioModule->getPacketLength();
    LOGGING(DEBUG, "Received something time to read it");

    rxLen = min(packetLength, sizeof(rxBuf));
    memcpy(rxBuf, receivedData, rxLen);

    currentFrameView.reset(rxBuf, rxLen);

    if (currentFrameView.validate() != ParseError::Ok) {
        Serial.println(static_cast<uint8_t>( currentFrameView.validate()));
        Serial.println(currentFrameView._len);
        Serial.println(sizeof(rxBuf));
        LOGGING( CRIT, "PROBLEMS with frame");
    }
}

void GroundStation::printVerboseTelemetryPacket()
{
    printAtomics( currentFrameView );
    
    // int RSSI = packetController->getRSSI();
    // int SNR = packetController->getSNR();
    // char dataStore[13] = {0};
    // snprintf(dataStore, sizeof(dataStore), ",%-4d,%-3d", RSSI, SNR);
    // Console.println(dataStore);
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
        matchCurrentRadioParamsWithRadioModule();
    }
}

GroundStation *GroundStation::instance = nullptr;
