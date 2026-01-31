#include "RadioChip.h"
#include <Arduino.h>
#include <LoggerGS.h>

// Constructor: attach module and decide which radio to use
RadioChip::RadioChip(Module *mod)
    : _radio1262(mod),
      _radio1268(mod),
      _freq(getFrequencyByBandPin())
{
    if (_freq == FREQUENCY_903)
    {
        _radio = &_radio1262;
    }
    else if (_freq == FREQUENCY_435)
    {
        _radio = &_radio1268;
    }
    else
    {
        Serial.println("Error initializing RadioChip: invalid frequency pin state");
        _radio = &_radio1262;
    }
}

int RadioChip::begin(float freq,
                     float bw,
                     int sf,
                     int cr,
                     int sw,
                     int po,
                     int pl,
                     float tcxo,
                     bool ldo)
{
    if (_freq == FREQUENCY_903)
    {
        return static_cast<SX1262 *>(_radio)
            ->begin(freq, bw, sf, cr, sw, po, pl, tcxo, ldo);
    }
    else
    {
        return static_cast<SX1268 *>(_radio)
            ->begin(freq, bw, sf, cr, sw, po, pl, tcxo, ldo);
    }
}

void RadioChip::setPacketReceivedAction(void (*func)())
{
    _radio->setPacketReceivedAction(func);
}

int RadioChip::startTransmit(const uint8_t *data, size_t len)
{
    return _radio->startTransmit(data, len);
}

int RadioChip::startReceive()
{
    return _radio->startReceive();
}

size_t RadioChip::getPacketLength()
{
    return _radio->getPacketLength();
}

int RadioChip::readData(uint8_t *data, size_t len)
{
    return _radio->readData(data, len);
}

int RadioChip::setFrequency(float freq)
{
    return _radio->setFrequency(freq);
}

int RadioChip::setBandwidth(float bw)
{
    return _radio->setBandwidth(bw);
}

int RadioChip::setSpreadingFactor(uint8_t sf)
{
    return _radio->setSpreadingFactor(sf);
}

int RadioChip::setCodingRate(uint8_t cr)
{
    return _radio->setCodingRate(cr);
}

int RadioChip::setOutputPower(int8_t power)
{
    if (_freq == FREQUENCY_903)
    {
        return static_cast<SX1262 *>(_radio)->setOutputPower(power);
    }
    else
    {
        return static_cast<SX1268 *>(_radio)->setOutputPower(power);
    }
}

int RadioChip::setCurrentLimit(float mA)
{
    if (_freq == FREQUENCY_903) {
        return static_cast<SX1262*>(_radio)->setCurrentLimit(mA);
    } else {
        return static_cast<SX1268*>(_radio)->setCurrentLimit(mA);
    }
}

float RadioChip::getRSSI()
{
    return _radio->getRSSI();
}

float RadioChip::getSNR()
{
    return _radio->getSNR();
}

float RadioChip::getFrequencyByBandPin()
{
    pinMode(FREQ_PIN, INPUT);
    LOGGING(DEBUG, "Checking freq pin: ");
    if (digitalRead(FREQ_PIN) == HIGH)
    {
        LOGGING(DEBUG, "HIGH so 903");
        return FREQUENCY_903;
    }
    else
    {
        LOGGING(DEBUG, "LOW so 435");
        return FREQUENCY_435;
    }
}