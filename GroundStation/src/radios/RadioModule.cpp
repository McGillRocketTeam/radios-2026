#include "RadioModule.h"

#include "ConsoleRouter.h"
#include "LoggerGS.h"
#include "ParamStore.h"

// Static member declarations for volatile variable used in isr
volatile bool RadioModule::interruptReceived = false;
volatile bool RadioModule::radioBusy = false;

// === Setup ===

RadioModule::RadioModule()
    : mod_(NSS_PIN, DIO1_PIN, RST_PIN, BUSY_PIN),
      radio_(&mod_),
      frequency(ParamStore::getDefaultBandFreq())
{
    // Default initiliased pins need to be set in the right mode
    pinMode(rxLedPin_, OUTPUT);
    pinMode(txLedPin_, OUTPUT);
    state_ = radio_.begin(frequency, bandwidth, spreadingFactor, codingRate, syncWord, powerOutput, preambleLength,
                          TCXO_VOLTAGE, USE_ONLY_LDO);
    while (!retryRadioInit())
    {
        Serial.println("radio init failure");
    }
    verifyRadioState("Intializing radio module...");
    radio_.setDio1Action(radioDio1ISR);
    radio_.setCurrentLimit(RADIO_CURRENT_LIMIT);
    memset(buffer, 0, sizeof(buffer));
}

RadioChip *RadioModule::getRadioChipInstance()
{
    return &radio_;
}

bool RadioModule::retryRadioInit()
{
    if (RadioStatus::ok(state_))
        return true;
    // TODO there is a bug here, where if we realise we picked the wrong freq
    // and call console.print it will print to the wrong topic as well, cause of the wrong freq
    // also this error is unique to 900 band, for now just use serial.print, and the router
    // to do debug this issue but future will need a better solution

    // Console.print("FATAL ERROR RADIO MODULE FAILED TO INTIALISE RADIOLIB ERROR CODE: ");
    // Console.println(state_);
    // Console.print("frequency is");
    // Console.println(frequency);
    // Console.print("if error persists try power cycling by disconnecting and reconnecting/ usb");

    Serial.print("FATAL ERROR RADIO MODULE FAILED TO INTIALISE RADIOLIB ERROR CODE: ");
    Serial.println(state_);
    Serial.print("frequency is");
    Serial.println(frequency);
    Serial.print("if error persists try power cycling by disconnecting and reconnecting/ usb");
    delay(250);
    frequency = ParamStore::getDefaultBandFreq();
    state_ = radio_.begin(frequency, bandwidth, spreadingFactor, codingRate, syncWord, powerOutput, preambleLength,
                          TCXO_VOLTAGE, USE_ONLY_LDO);
    return false;
}

// === MAIN functions ===

bool RadioModule::transmitBlocking(const uint8_t *data, size_t size)
{
    radioBusy = true;

    // clear previous interrupt latch
    noInterrupts();
    interruptReceived = false;
    interrupts();

    // clear radio IRQ flags
    state_ = radio_.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
    if (!verifyRadioState("clearIrqFlags before TX"))
    {
        radioBusy = false;
        return false;
    }

    // start transmit (non-blocking)
    state_ = radio_.startTransmit(data, size);
    if (!verifyRadioState("startTransmit"))
    {
        radioBusy = false;
        return false;
    }

    // wait for ISR to fire
    uint32_t start = millis();

    while (!interruptReceived)
    {
        // TODO we should calculate the timeout threshold based on toa
        if (millis() - start > 200)
        {

            LOGGING(CAT_RADIO, CRIT, "TX wait timeout");

            // wait for chip to become ready for SPI commands
            while (digitalRead(BUSY_PIN))
                ;

            // force radio into known state
            state_ = radio_.standby();
            verifyRadioState("standby after TX timeout");

            // clear stale interrupts
            state_ = radio_.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
            verifyRadioState("clearIrqFlags after TX timeout");

            // restart RX
            state_ = radio_.startReceive();
            verifyRadioState("startReceive after TX timeout");
            // TODO some sort of ultimiate fail safe if startReceive dosnt work
            // Probably a hard reset of the radio,and then a hard restart of the system
            radioBusy = false;
            return false;
        }
    }

    // consume latch
    noInterrupts();
    interruptReceived = false;
    interrupts();

    // read IRQ flags
    uint32_t flags = radio_.getIrqFlags();

    const uint32_t TX_DONE = (1UL << RADIOLIB_IRQ_TX_DONE);

    if ((flags & TX_DONE) == 0)
    {
        LOGGING(CAT_RADIO, CRIT, "TX interrupt but TX_DONE not set");
        radio_.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
        radio_.startReceive();
        radioBusy = false;
        return false;
    }

    // clear flags
    radio_.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);

    // switch back to RX
    state_ = radio_.startReceive();
    verifyRadioState("startReceive after TX");

    toggleLedOnOk(txLedPin_);

    radioBusy = false;

    return true;
}

bool RadioModule::pollValidPacketRx()
{
    if (radioBusy)
    {
        LOGGING(CAT_GS, CRIT, "Trying to read receive while busy TX this should never happen");
        delay(10);
        return false;
    }
    // Atomically consume ISR latch
    noInterrupts();
    bool hadIrq = interruptReceived;
    interruptReceived = false;
    interrupts();

    if (!hadIrq)
        return false;

    const uint32_t flags = radio_.getIrqFlags();

    const uint32_t RX_DONE = (1UL << RADIOLIB_IRQ_RX_DONE);
    const uint32_t CRC_ERR = (1UL << RADIOLIB_IRQ_CRC_ERR);
    const uint32_t HDR_ERR = (1UL << RADIOLIB_IRQ_HEADER_ERR);
    const uint32_t TIMEOUT = (1UL << RADIOLIB_IRQ_TIMEOUT);

    // Not a RX_DONE
    if ((flags & RX_DONE) == 0)
    {
        radio_.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
        radio_.startReceive();
        return false;
    }

    // Not a valid payload check
    if (flags & (CRC_ERR | HDR_ERR | TIMEOUT))
    {
        if (flags & CRC_ERR)
        {
            LOGGING(CAT_RADIO, CRIT, String("RX CRC error, RSSI: ") + String(getRSSI(), 2) + ", SNR: " + String(getSNR(), 2));
        }
        else if (flags & HDR_ERR)
        {
            LOGGING(CAT_RADIO, CRIT, "RX header error");
        }
        else
        {
            LOGGING(CAT_RADIO, DEBUG, "RX timeout");
        }

        radio_.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
        radio_.startReceive();
        return false;
    }

    // Valid packet read it now and set the buffer
    const size_t n = radio_.getPacketLength();
    if (n == 0 || n > sizeof(buffer))
    {
        LOGGING(CAT_RADIO, CRIT, String("RX length invalid: ") + String((int)n));
        state_ = radio_.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
        state_ = radio_.startReceive();
        return false;
    }

    lastPacketLength = (int)n;
    memset(buffer, 0, sizeof(buffer));

    state_ = radio_.readData(buffer, n);

    // If RadioLib still reports CRC mismatch here, treat as invalid.
    if (!RadioStatus::ok(state_))
    {
        verifyRadioState("readData");
        radio_.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
        radio_.startReceive();
        return false;
    }

    state_ = radio_.clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
    verifyRadioState("clearIrqFlags");

    state_ = radio_.startReceive();
    verifyRadioState("startReceive");

    toggleLedOnOk(rxLedPin_);

    // buffer now contains a valid packet
    return true;
}

bool RadioModule::receiveMode()
{
    state_ = radio_.startReceive();
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
    return radio_.getRSSI();
}

uint8_t RadioModule::getRawRSSI()
{
    float f = radio_.getRSSI() * -2.0;
    // clamp rogue values, to match the register size
    if (f < 0.0f)
        f = 0.0f;
    if (f > 255.0f)
        f = 255.0f;

    return static_cast<uint8_t>(f);
}

float RadioModule::getSNR()
{
    return radio_.getSNR();
}

int8_t RadioModule::getRawSNR()
{
    float f = radio_.getRSSI() * 4.0;

    // clamp rogue values, to match the register size
    if (f < -125.0f)
        f = -125.0f;
    if (f > 125.0f)
        f = 125.0f;

    return static_cast<int8_t>(f);
}

// === General Helper ===

void RadioModule::toggleLedOnOk(int pin)
{
    if (!RadioStatus::ok(state_))
    {
        return;
    }
    if (pin == rxLedPin_)
    {
        rxLedState_ = !rxLedState_;
        digitalWrite(pin, rxLedState_);
    }
    else if (pin == txLedPin_)
    {
        txLedState_ = !txLedState_;
        digitalWrite(pin, txLedState_);
    }
    else
    {
        LOGGING(CAT_RADIO, INFO, "Toggling a LED that is not set in the radio module");
    }
}

bool RadioModule::verifyRadioState(String message)
{
    if (RadioStatus::ok(state_))
    {
        LOGGING(CAT_RADIO, DEBUG, message + " Success!");
        return true;
    }
    else if (RadioStatus::crcErr(state_))
    {
        // If its a crc error we can log the SNR and RSSI
        // of the packet received even though its data is cooked
        char buf[64];
        // RSSI in dBm SNR in db
        snprintf(buf, sizeof(buf),
                 "CRC error, RSSI: %.2f, SNR: %.2f",
                 getRSSI(), getSNR());
        LOGGING(CAT_RADIO, CRIT, buf);
        return false;
    }

    LOGGING(CAT_RADIO, CRIT, message + " Failure. Error code: " + String(state_));
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
    state_ = radio_.setFrequency(newFrequency);
    if (verifyRadioState("Switching to frequency of " + String(newFrequency)))
    {
        frequency = newFrequency;
    }
}

void RadioModule::setBandwidth(float newBandwidth)
{
    state_ = radio_.setBandwidth(newBandwidth);
    if (verifyRadioState("Switching to bandwidth of " + String(newBandwidth)))
    {
        bandwidth = newBandwidth;
    }
}

void RadioModule::setSpreadingFactor(uint8_t newSpreadingFactor)
{
    state_ = radio_.setSpreadingFactor(newSpreadingFactor);
    if (verifyRadioState("Switching to spreading factor of " + String(newSpreadingFactor)))
    {
        spreadingFactor = newSpreadingFactor;
    }
}

void RadioModule::setCodingRate(uint8_t newCodingRate)
{
    state_ = radio_.setCodingRate(newCodingRate);
    if (verifyRadioState("Switching to coding rate of " + String(newCodingRate)))
    {
        codingRate = newCodingRate;
    }
}

void RadioModule::setPowerOutput(int8_t newPowerOutput)
{
    state_ = radio_.setOutputPower(newPowerOutput);
    if (verifyRadioState("Switching to power output of " + String(newPowerOutput)))
    {
        powerOutput = newPowerOutput;
    }
}
