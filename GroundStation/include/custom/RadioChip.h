#ifndef RADIOCHIP_H
#define RADIOCHIP_H

#include <RadioLib.h>
#include "Config.h"
#include <stdexcept>
#include "PinLayout.h"

/**
 * @class RadioChip
 * @brief Wrapper class for handling RadioLib SX126x series chips (SX1262/SX1268).
 *
 * Provides a unified interface to initialize, configure, and communicate
 * with LoRa radio modules using RadioLib. Automatically selects between
 * SX1262 or SX1268 based on pin configuration or hardware.
 */
class RadioChip {
public:
    /**
     * @brief Construct a new RadioChip object with the provided module.
     * @param mod Pointer to the Module object for RadioLib.
     */
    RadioChip(Module *mod);

    /**
     * @brief Initialize the radio with the specified parameters.
     * @param freq Frequency in MHz (e.g., 915.0).
     * @param bw Bandwidth (e.g., 125.0).
     * @param sf Spreading factor (e.g., 7–12).
     * @param cr Coding rate (1–4 for 4/5 to 4/8).
     * @param sw Sync word.
     * @param po Output power in dBm.
     * @param pl Preamble length.
     * @param tcxo TCXO voltage.
     * @param ldo Use LDO only (true) or allow TCXO use (false).
     * @return RadioLib status code (0 = success).
     */
    int begin(float freq,
              float bw,
              int sf,
              int cr,
              int sw,
              int po,
              int pl,
              float tcxo,
              bool ldo);

    /**
     * @brief Assign a callback function to be called when a packet is received.
     * @param func Pointer to a no-argument ISR-style function.
     */
    void setPacketReceivedAction(void (*func)());

    /**
     * @brief Begin asynchronous transmission of a byte buffer.
     * @param data Pointer to the data buffer to send.
     * @param len Length of the buffer in bytes.
     * @return RadioLib status code.
     */
    int startTransmit(const uint8_t *data, size_t len);

    /**
     * @brief Start receiving packets asynchronously.
     * @return RadioLib status code.
     */
    int startReceive();

    /**
     * @brief Get the length of the last received packet.
     * @return Size of the received packet in bytes.
     */
    size_t getPacketLength();

    /**
     * @brief Read received data into the provided buffer.
     * @param data Buffer to populate with received bytes.
     * @param len Maximum number of bytes to read.
     * @return RadioLib status code.
     */
    int readData(uint8_t *data, size_t len);

    /**
     * @brief Set the frequency of the radio.
     * @param freq Frequency in MHz.
     * @return RadioLib status code.
     */
    int setFrequency(float freq);

    /**
     * @brief Set the bandwidth of the radio.
     * @param bw Bandwidth in kHz.
     * @return RadioLib status code.
     */
    int setBandwidth(float bw);

    /**
     * @brief Set the spreading factor.
     * @param sf Spreading factor (e.g., 7–12).
     * @return RadioLib status code.
     */
    int setSpreadingFactor(uint8_t sf);

    /**
     * @brief Set the coding rate.
     * @param cr Coding rate (1–4).
     * @return RadioLib status code.
     */
    int setCodingRate(uint8_t cr);

    /**
     * @brief Set the radio output power.
     * @param power Output power in dBm.
     * @return RadioLib status code.
     */
    int setOutputPower(int8_t power);

    /**
     * @brief Get the RSSI (Received Signal Strength Indicator) of the last packet.
     * @return RSSI in dBm.
     */
    int32_t getRSSI();

    /**
     * @brief Get the SNR (Signal-to-Noise Ratio) of the last packet.
     * @return SNR in dB.
     */
    int32_t getSNR();

    /**
     * @brief Derives the radio frequency based on BAND_PIN hardware config.
     * @return Frequency in MHz (e.g., 435.00 or 903.00).
     */
    float getFrequencyByBandPin();

private:
    /// RadioLib SX1262 driver instance
    SX1262 _radio1262;
    /// RadioLib SX1268 driver instance     
    SX1268 _radio1268;
    /// Generic pointer to selected SX126x driver     
    SX126x *_radio;
    /// Currently configured frequency        
    float _freq;           
};

#endif // RADIOCHIP_H
