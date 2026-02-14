#include "RadioChip.h"

#include <Arduino.h>

#include "BandSelect.h"
#include "LoggerGS.h"

// Constructor: attach module and decide which radio to use
RadioChip::RadioChip(Module *mod)
    : _radio1262(mod),
      _radio1268(mod)
{
    if (BandSelect::is903())
    {
        _radio = &_radio1262;
    }
    else if (BandSelect::is435())
    {
        _radio = &_radio1268;
    }
    else
    {
        Serial.println("Error initializing RadioChip: invalid frequency pin state");
        _radio = &_radio1262;
    }
}

RadioChipStatus RadioChip::begin(float freq,
                     float bw,
                     int sf,
                     int cr,
                     int sw,
                     int po,
                     int pl,
                     float tcxo,
                     bool ldo)
{
    if (!BandSelect::freqAllowedFromBand(freq)) {
        Serial.println("THIS FREQUENCY IS NOT AN ALLOWED BAND");
        return RADIOLIB_ERR_INVALID_FREQUENCY;
    }

    if (BandSelect::is903()) {
        return _radio1262.begin(freq, bw, sf, cr, sw, po, pl, tcxo, ldo);
    } else if (BandSelect::is435()) {
        return _radio1268.begin(freq, bw, sf, cr, sw, po, pl, tcxo, ldo);
    }
    return RADIOLIB_ERR_INVALID_FREQUENCY;
}

void RadioChip::setDio1Action(void (*func)())
{
    _radio->setDio1Action(func);
}

uint32_t RadioChip::getIrqFlags() {
    return _radio->getIrqFlags();
}

RadioChipStatus RadioChip::clearIrqFlags(uint32_t mask) {
    return _radio->clearIrqFlags(mask);
}


RadioChipStatus RadioChip::transmit(const uint8_t *data, size_t len)
{
    return _radio->transmit(data, len);
}

RadioChipStatus RadioChip::startReceive()
{
    return _radio->startReceive();
}

RadioChipStatus RadioChip::standby(){
    return _radio->standby();
}

size_t RadioChip::getPacketLength()
{
    return _radio->getPacketLength();
}

RadioChipStatus RadioChip::readData(uint8_t *data, size_t len)
{
    return _radio->readData(data, len);
}

RadioChipStatus RadioChip::setFrequency(float freq)
{
    if (!BandSelect::freqAllowedFromBand(freq)) {
        Serial.println("THIS FREQUENCY IS NOT AN ALLOWED BAND");
        return RADIOLIB_ERR_INVALID_FREQUENCY;
    }
    return _radio->setFrequency(freq);
}

RadioChipStatus RadioChip::setBandwidth(float bw)
{
    return _radio->setBandwidth(bw);
}

RadioChipStatus RadioChip::setSpreadingFactor(uint8_t sf)
{
    return _radio->setSpreadingFactor(sf);
}

RadioChipStatus RadioChip::setCodingRate(uint8_t cr)
{
    return _radio->setCodingRate(cr);
}

RadioChipStatus RadioChip::setOutputPower(int8_t power)
{
    return _radio->setOutputPower(power);
}

RadioChipStatus RadioChip::setCurrentLimit(float mA)
{
    return _radio->setCurrentLimit(mA);
}

float RadioChip::getRSSI()
{
    return _radio->getRSSI();
}

float RadioChip::getSNR()
{
    return _radio->getSNR();
}