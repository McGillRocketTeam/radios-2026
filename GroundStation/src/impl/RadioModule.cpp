#include <RadioModule.h>
#include <LoggerGS.h>
#include <ConsoleRouter.h>

// Static member declarations for volatile variable used in isr
volatile bool RadioModule::interruptReceived = false;
volatile bool RadioModule::enableInterrupt = true;

RadioModule::RadioModule()
    : radio(new Module(NSS_PIN, DIO1_PIN, RST_PIN, BUSY_PIN))
{
    state = radio.begin(frequency, bandwidth, spreadingFactor, codingRate, syncWord, powerOutput, preambleLength,
                        TCXO_VOLTAGE, USE_ONLY_LDO);
    while (state != RADIOLIB_ERR_NONE)
    {
        Console.print("FATAL ERROR RADIO MODULE FAILED TO INTIALISE RADIOLIB ERROR CODE: ");
        Console.println(state);
        Console.print("frequency is");
        Console.println(frequency);
        Console.print("if error persists try power cycling by disconnecting and reconnecting/ usb");
        if (frequency == FREQUENCY_435){
            Console.println("Check the battery?");
            Console.println("Checking the freq pin again");
        }
        delay(250);
        frequency = getFrequencyByBandPin();
        state = radio.begin(frequency, bandwidth, spreadingFactor, codingRate, syncWord, powerOutput, preambleLength,
                        TCXO_VOLTAGE, USE_ONLY_LDO);
    }
    verifyRadioState("Intializing radio module...");
    radio.setPacketReceivedAction(isr);
    memset(buffer, 0, sizeof(buffer));
}

// Transmit data array of bytes and stay in transmit mode
bool RadioModule::transmitInterrupt(const uint8_t *data, size_t size)
{
    state = radio.startTransmit(data, size);
    verifyRadioState("Attempting to transmit... ");
    while (interruptReceived == false)
    {
        delay(10);
        LOGGING(DEBUG_RADIO,"waiting for data to transmit");
    }
    interruptReceived = false;
    return true;
}

bool RadioModule::receiveMode()
{
    state = radio.startReceive();
    return verifyRadioState("Switching into receive mode... ");
}

bool RadioModule::checkInterruptReceived()
{
    return interruptReceived;
}

uint8_t *RadioModule::readPacket()
{
    size_t packetLength = radio.getPacketLength();
    lastPacketLength = packetLength;
    // Intialise the buffer to be zero
    memset(buffer, 0, sizeof(buffer));
    state = radio.readData(buffer, packetLength);
    verifyRadioState("Attempting to read data...");
    interruptReceived = false;
    return buffer;
}

int RadioModule::getPacketLength()
{
    return lastPacketLength;
}

float RadioModule::getFreq()
{
    return frequency;
}

float RadioModule::getBandwidth()
{
    return bandwidth;
}

int RadioModule::getSpreadingFactor()
{
    return spreadingFactor;
}

int RadioModule::getCodingRate()
{
    return codingRate;
}

int RadioModule::getPowerOutput()
{
    return powerOutput;
}

void RadioModule::setFreq(float newFrequency)
{
    state = radio.setFrequency(newFrequency);
    if (verifyRadioState("Switching to frequency of " + String(newFrequency)))
    {
        frequency = newFrequency;
    }
}

void RadioModule::setBandwidth(float newBandwidth)
{
    state = radio.setBandwidth(newBandwidth);
    if (verifyRadioState("Switching to bandwidth of " + String(newBandwidth)))
    {
        bandwidth = newBandwidth;
    }
}

void RadioModule::setSpreadingFactor(int newSpreadingFactor)
{
    state = radio.setSpreadingFactor(newSpreadingFactor);
    if (verifyRadioState("Switching to spreading factor of " + String(newSpreadingFactor)))
    {
        spreadingFactor = newSpreadingFactor;
    }
}

void RadioModule::setCodingRate(int newCodingRate)
{
    state = radio.setCodingRate(newCodingRate);
    if (verifyRadioState("Switching to coding rate of " + String(newCodingRate)))
    {
        codingRate = newCodingRate;
    }
}

void RadioModule::setPowerOutput(int newPowerOutput)
{
    state = radio.setOutputPower(newPowerOutput);
    if (verifyRadioState("Switching to power output of " + String(newPowerOutput)))
    {
        powerOutput = newPowerOutput;
    }
}

void RadioModule::setDebug(bool debugDesired)
{
    LoggerGS::getInstance().setEnabled(DEBUG_RADIO,debugDesired);
}

void RadioModule::checkParams()
{
    Console.print("frequency: ");
    Console.print(frequency);

    Console.print(" bandwidth: ");
    Console.print(bandwidth);

    Console.print(" spreading factor: ");
    Console.print(spreadingFactor);

    Console.print(" coding rate: ");
    Console.print(codingRate);

    Console.print(" power output: ");
    Console.print(powerOutput);

    Console.print(" debug state: ");
    Console.println(LoggerGS::getInstance().isEnabled(DEBUG_RADIO));
}

void RadioModule::pingParams()
{
    //ping_ack:8,250.00,903.00,7,20
    Console.print("ping_ack");
    Console.print(":");
    Console.print(spreadingFactor);
    Console.print(",");
    Console.print(bandwidth);
    Console.print(",");
    Console.print(frequency);
    Console.print(",");
    Console.print(codingRate);
    Console.print(",");
    Console.println(powerOutput);
}

RadioChip *RadioModule::getRadioChipInstance()
{
    return &radio;
}

int RadioModule::getRSSI()
{
    return radio.getRSSI();
}

int RadioModule::getSNR()
{
    return radio.getSNR();
}

float RadioModule::getFrequencyByBandPin()
{
    if (digitalRead(FREQ_PIN) == HIGH)
    {
        LOGGING(DEBUG_RADIO,"FREQ_PIN is HIGH using 903.00 MHz");
        return FREQUENCY_903;
    }
    else
    {
        LOGGING(DEBUG_RADIO,"FREQ_PIN is LOW using 435.00 MHz");
        return FREQUENCY_435;
    }
}


bool RadioModule::verifyRadioState(String message)
{
    if (state == RADIOLIB_ERR_NONE)
    {
        LOGGING(DEBUG_RADIO,message + " Success!");
        return true;
    }
    else
    {
        LOGGING(DEBUG_RADIO,message + " Failure. Error code: " + String(state));
        return false;
    }
}

void RadioModule::isr()
{
    if (RadioModule::enableInterrupt)
    {
        RadioModule::interruptReceived = true;
    }
}
