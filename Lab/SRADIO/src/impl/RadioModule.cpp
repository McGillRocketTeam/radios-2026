#include "RadioModule.h"

// Static member declarations for volatile variable used in isr
volatile bool RadioModule::interruptReceived = false;
volatile bool RadioModule::enableInterrupt = true;

// === Setup ===

RadioModule::RadioModule()
    : radio(new Module(NSS_PIN, DIO1_PIN, RST_PIN, BUSY_PIN))
{
    state = radio.begin(frequency, bandwidth, spreadingFactor, codingRate, syncWord, powerOutput, preambleLength,
                        TCXO_VOLTAGE, USE_ONLY_LDO);
    while (state != RADIOLIB_ERR_NONE)
    {
        Serial.print("FATAL ERROR RADIO MODULE FAILED TO INTIALISE RADIOLIB ERROR CODE: ");
        Serial.println(state);
        Serial.print("frequency is");
        Serial.println(frequency);
        Serial.print("if error persists try power cycling by disconnecting and reconnecting/ usb");
        if (frequency == FREQUENCY_435)
        {
            Serial.println("Check the battery?");
            Serial.println("Checking the freq pin again");
        }
        delay(500);
        frequency = getFrequencyByBandPin();
        state = radio.begin(frequency, bandwidth, spreadingFactor, codingRate, syncWord, powerOutput, preambleLength,
                            TCXO_VOLTAGE, USE_ONLY_LDO);
    }
    verifyRadioState("Intializing radio module...");
    radio.setPacketReceivedAction(radioReceiveISR);
    memset(buffer, 0, sizeof(buffer));
}

RadioChip *RadioModule::getRadioChipInstance()
{
    return &radio;
}

// === MAIN functions ===

bool RadioModule::transmitInterrupt(const uint8_t *data, size_t size)
{
    interruptReceived = false;

    state = radio.startTransmit(data, size);
    verifyRadioState("Attempting to transmit... ");

    const uint32_t timeoutMs = 1000;
    const uint32_t start = millis();
    uint32_t lastLog = 0;

    while (!interruptReceived)
    {
        if (millis() - start > timeoutMs)
        {
            verifyRadioState("ERROR: TX timeout");
            return false;
        }

        if (millis() - lastLog > 250)
        {
            Serial.println("waiting for TX done IRQ...");
            lastLog = millis();
        }

        delay(2);
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
    memset(buffer, 0, sizeof(buffer));
    state = radio.readData(buffer, packetLength);
    interruptReceived = false;
    
    if (state == RADIOLIB_ERR_NONE)
    {
    }
    else if (state == RADIOLIB_ERR_CRC_MISMATCH)
    {
        // If its a crc error we can log the SNR and RSSI
        // of the packet received even though its data is cooked
        char buf[64];
        // RSSI in dBm SNR in db
        snprintf(buf, sizeof(buf),
                 " ,CRC error, RSSI: %.2f, SNR: %.2f",
                 getRSSI(), getSNR());
        Serial.println(buf);
        return nullptr;
    }
    else
    {
        Serial.println("Read Failure. Error code: " + String(state));
        return nullptr;
    }

    return buffer;
}

void RadioModule::radioReceiveISR()
{
    if (RadioModule::enableInterrupt)
    {
        RadioModule::interruptReceived = true;
    }
}

// === Parameters Helpers ===

void RadioModule::checkParams()
{
    Serial.print("frequency: ");
    Serial.print(frequency);

    Serial.print(" bandwidth: ");
    Serial.print(bandwidth);

    Serial.print(" spreading factor: ");
    Serial.print(spreadingFactor);

    Serial.print(" coding rate: ");
    Serial.print(codingRate);

    Serial.print(" power output: ");
    Serial.print(powerOutput);
}

void RadioModule::pingParams()
{
    // ping_ack:8,250.00,903.00,7,20
    Serial.print("ping_ack");
    Serial.print(":");
    Serial.print(spreadingFactor);
    Serial.print(",");
    Serial.print(bandwidth);
    Serial.print(",");
    Serial.print(frequency);
    Serial.print(",");
    Serial.print(codingRate);
    Serial.print(",");
    Serial.println(powerOutput);
}

float RadioModule::getFrequencyByBandPin()
{
    if (digitalRead(FREQ_PIN) == HIGH)
    {
        Serial.println("FREQ_PIN is HIGH using 903.00 MHz");
        return FREQUENCY_903;
    }
    else
    {
        Serial.println("FREQ_PIN is LOW using 435.00 MHz");
        return FREQUENCY_435;
    }
}

// === Packet Getters Setters ===

int RadioModule::getPacketLength()
{
    return lastPacketLength;
}

float RadioModule::getRSSI()
{
    return radio.getRSSI();
}

float RadioModule::getSNR()
{
    return radio.getSNR();
}

// === General Helper ===

bool RadioModule::verifyRadioState(String message)
{
    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println(message + " Success!");
        return true;
    }
    else if (state == RADIOLIB_ERR_CRC_MISMATCH)
    {
        char buf[64];
        // RSSI in dBm SNR in db
        snprintf(buf, sizeof(buf),
                 " ,CRC error, RSSI: %.2f, SNR: %.2f",
                 getRSSI(), getSNR());
        Serial.println(buf);
        return false;
    }
    else
    {
        Serial.println(message + " Failure. Error code: " + String(state));
        return false;
    }
}

// === Radio Chip Getters Setters ===

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
