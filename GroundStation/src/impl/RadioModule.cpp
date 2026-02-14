#include "RadioModule.h"

#include "BandSelect.h"
#include "ConsoleRouter.h"
#include "LoggerGS.h"

// Static member declarations for volatile variable used in isr
volatile bool RadioModule::interruptReceived = false;
volatile bool RadioModule::radioBusy = false;

// === Setup ===

RadioModule::RadioModule()
    : mod(NSS_PIN, DIO1_PIN, RST_PIN, BUSY_PIN),
      radio(&mod),
      frequency(BandSelect::get())
{
    state = radio.begin(frequency, bandwidth, spreadingFactor, codingRate, syncWord, powerOutput, preambleLength,
                        TCXO_VOLTAGE, USE_ONLY_LDO);
    while (!retryRadioInit())
    {
        Serial.println("radio init failure");
    }
    verifyRadioState("Intializing radio module...");
    radio.setDio1Action(radioDio1ISR);
    radio.setCurrentLimit(RADIO_CURRENT_LIMIT);
    memset(buffer, 0, sizeof(buffer));
}

RadioChip *RadioModule::getRadioChipInstance()
{
    return &radio;
}

bool RadioModule::retryRadioInit()
{
    if (RadioStatus::ok(state))
        return true;
    Console.print("FATAL ERROR RADIO MODULE FAILED TO INTIALISE RADIOLIB ERROR CODE: ");
    Console.println(state);
    Console.print("frequency is");
    Console.println(frequency);
    Console.print("if error persists try power cycling by disconnecting and reconnecting/ usb");
    if (frequency == FREQUENCY_435)
    {
        Console.println("Check the battery?");
        Console.println("Checking the freq pin again");
    }
    delay(250);
    frequency = BandSelect::get();
    state = radio.begin(frequency, bandwidth, spreadingFactor, codingRate, syncWord, powerOutput, preambleLength,
                        TCXO_VOLTAGE, USE_ONLY_LDO);
    return false;
}

// === MAIN functions ===

bool RadioModule::transmitBlocking(const uint8_t* data, size_t size)
{
    radioBusy = true;

    noInterrupts();
    interruptReceived = false;
    interrupts();
    radio.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);

    state = radio.transmit(data, size);
    bool ok = RadioStatus::ok(state);

    radio.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
    radio.startReceive();

    radioBusy = false;

    return ok;
}


bool RadioModule::pollValidPacketRx()
{
    if (radioBusy) {
        Serial.println("aslkfnafd");
        delay(1000);
        return false;
    }
    // Atomically consume ISR latch
    noInterrupts();
    bool hadIrq = interruptReceived;
    interruptReceived = false;
    interrupts();

    if (!hadIrq)
        return false;

    const uint32_t flags = radio.getIrqFlags();

    const uint32_t RX_DONE = (1UL << RADIOLIB_IRQ_RX_DONE);
    const uint32_t CRC_ERR = (1UL << RADIOLIB_IRQ_CRC_ERR);
    const uint32_t HDR_ERR = (1UL << RADIOLIB_IRQ_HEADER_ERR);
    const uint32_t TIMEOUT = (1UL << RADIOLIB_IRQ_TIMEOUT);

    // Not a RX_DONE
    if ((flags & RX_DONE) == 0)
    {
        radio.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
        radio.startReceive();
        return false;
    }

    // Not a vlid payload check
    if (flags & (CRC_ERR | HDR_ERR | TIMEOUT))
    {
        if (flags & CRC_ERR)
        {
            LOGGING(CAT_RADIO,CRIT, String("RX CRC error, RSSI: ") + String(getRSSI(), 2) +
                              ", SNR: " + String(getSNR(), 2));
        }
        else if (flags & HDR_ERR)
        {
            LOGGING(CAT_RADIO,CRIT, "RX header error");
        }
        else
        {
            LOGGING(CAT_RADIO,DEBUG, "RX timeout");
        }

        radio.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
        radio.startReceive();
        return false;
    }

    // Valid packet read it now and set the buffer
    const size_t n = radio.getPacketLength();
    if (n == 0 || n > sizeof(buffer))
    {
        LOGGING(CAT_RADIO,CRIT, String("RX length invalid: ") + String((int)n));
        state = radio.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
        state = radio.startReceive();
        return false;
    }

    lastPacketLength = (int)n;
    memset(buffer, 0, sizeof(buffer));

    state = radio.readData(buffer, n);

    // If RadioLib still reports CRC mismatch here, treat as invalid.
    if (!RadioStatus::ok(state))
    {
        verifyRadioState("readData");
        radio.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
        radio.startReceive();
        return false;
    }

    state = radio.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
    verifyRadioState("clearIrqFlags");

    state = radio.startReceive();
    verifyRadioState("startReceive");

    // buffer now contains a valid packet
    return true;
}

bool RadioModule::receiveMode()
{
    state = radio.startReceive();
    return verifyRadioState("Switching into receive mode... ");
}

void RadioModule::radioDio1ISR()
{
    RadioModule::interruptReceived = true;
}

// === Parameters Helpers ===

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
}

void RadioModule::pingParams()
{
    // ping_ack:8,250.00,903.00,7,20
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

// === Packet Getters Setters ===

int RadioModule::getPacketLength()
{
    return lastPacketLength;
}

uint8_t *RadioModule::getPacketData()
{
    return buffer;
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
    if (RadioStatus::ok(state))
    {
        LOGGING(CAT_RADIO,DEBUG, message + " Success!");
        return true;
    }
    else if (RadioStatus::crcErr(state))
    {
        // If its a crc error we can log the SNR and RSSI
        // of the packet received even though its data is cooked
        char buf[64];
        // RSSI in dBm SNR in db
        snprintf(buf, sizeof(buf),
                 "CRC error, RSSI: %.2f, SNR: %.2f",
                 getRSSI(), getSNR());
        LOGGING(CAT_RADIO,CRIT, buf);
        return false;
    }

    LOGGING(CAT_RADIO,CRIT, message + " Failure. Error code: " + String(state));
    return false;
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
