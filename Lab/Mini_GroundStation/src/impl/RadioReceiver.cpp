#include "RadioReceiver.h"

#include "LoraParamConfig.h"

volatile bool RadioReceiver::interruptReceived_ = false;

RadioReceiver::RadioReceiver()
    : module_(NSS_PIN, DIO1_PIN, RST_PIN, BUSY_PIN),
      radio903_(&module_),
      radio435_(&module_)
{
}

bool RadioReceiver::begin()
{
    ready_ = false;

    if (!bandDetected_)
    {
        pinMode(FREQ_PIN, INPUT);
        delay(400);
        frequencyPinValue_ = analogRead(FREQ_PIN);
        bandDetected_ = true;
    }

    const bool use903Band = frequencyPinValue_ > 50;
    frequency_ = use903Band ? FREQUENCY_903 : FREQUENCY_435;
    radio_ = use903Band ? static_cast<SX126x*>(&radio903_)
                        : static_cast<SX126x*>(&radio435_);

    Serial.print("Radio band: ");
    Serial.print(frequency_, 2);
    Serial.print(" MHz (frequency pin ");
    Serial.print(frequencyPinValue_);
    Serial.println(")");

    const auto beginDetectedRadio = [this](auto& detectedRadio) {
        return detectedRadio.begin(
            frequency_,
            BANDWIDTH_USED,
            SPREADING_FACTOR_USED,
            CODING_RATE_USED,
            SYNC_WORD,
            POWER_OUTPUT,
            PREAMBLE_LENGTH,
            TCXO_VOLTAGE,
            USE_ONLY_LDO);
    };

    int16_t state = use903Band
        ? beginDetectedRadio(radio903_)
        : beginDetectedRadio(radio435_);
    if (state != RADIOLIB_ERR_NONE)
    {
        logRadioError("begin", state);
        return false;
    }

    radio_->setDio1Action(onDio1Interrupt);

    state = radio_->setCurrentLimit(RADIO_CURRENT_LIMIT);
    if (state != RADIOLIB_ERR_NONE)
    {
        logRadioError("setCurrentLimit", state);
        return false;
    }

    noInterrupts();
    interruptReceived_ = false;
    interrupts();

    ready_ = restartReceive();
    return ready_;
}

bool RadioReceiver::poll(TelemetryStore& telemetry)
{
    if (!ready_ || radio_ == nullptr)
    {
        return false;
    }

    noInterrupts();
    const bool hadInterrupt = interruptReceived_;
    interruptReceived_ = false;
    interrupts();

    if (!hadInterrupt)
    {
        return false;
    }

    const uint32_t flags = radio_->getIrqFlags();
    const uint32_t rxDone = (1UL << RADIOLIB_IRQ_RX_DONE);
    const uint32_t receiveErrors =
        (1UL << RADIOLIB_IRQ_CRC_ERR) |
        (1UL << RADIOLIB_IRQ_HEADER_ERR) |
        (1UL << RADIOLIB_IRQ_TIMEOUT);

    if ((flags & rxDone) == 0 || (flags & receiveErrors) != 0)
    {
        if ((flags & (1UL << RADIOLIB_IRQ_CRC_ERR)) != 0)
        {
            Serial.print("Radio RX CRC error, RSSI ");
            Serial.print(radio_->getRSSI(), 2);
            Serial.print(" dBm, SNR ");
            Serial.print(radio_->getSNR(), 2);
            Serial.println(" dB");
        }
        ready_ = restartReceive();
        return false;
    }

    const size_t packetLength = radio_->getPacketLength();
    if (packetLength == 0 || packetLength > sizeof(buffer_))
    {
        Serial.print("Radio RX invalid packet length: ");
        Serial.println(static_cast<unsigned>(packetLength));
        ready_ = restartReceive();
        return false;
    }

    const int16_t state = radio_->readData(buffer_, packetLength);
    if (state != RADIOLIB_ERR_NONE)
    {
        logRadioError("readData", state);
        ready_ = restartReceive();
        return false;
    }

    ready_ = restartReceive();
    if (!ready_)
    {
        return false;
    }

    if (!telemetry.updateFromFrame(buffer_, packetLength))
    {
        Serial.println("Radio RX ignored a non-telemetry or invalid ASTRA frame");
        return false;
    }

    Serial.print("Radio RX telemetry seq ");
    Serial.println(telemetry.latest().packet_sequence_number);
    return true;
}

bool RadioReceiver::isReady() const
{
    return ready_;
}

void RadioReceiver::onDio1Interrupt()
{
    interruptReceived_ = true;
}

bool RadioReceiver::restartReceive()
{
    int16_t state = radio_->clearIrqFlags(RADIOLIB_SX126X_IRQ_ALL);
    if (state != RADIOLIB_ERR_NONE)
    {
        logRadioError("clearIrqFlags", state);
        return false;
    }

    state = radio_->startReceive();
    if (state != RADIOLIB_ERR_NONE)
    {
        logRadioError("startReceive", state);
        return false;
    }

    return true;
}

void RadioReceiver::logRadioError(const char* operation, int16_t state) const
{
    Serial.print("Radio ");
    Serial.print(operation);
    Serial.print(" failed with RadioLib error ");
    Serial.println(state);
}
