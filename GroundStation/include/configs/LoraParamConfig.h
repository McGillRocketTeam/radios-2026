#pragma once

#include <cstdint>

// Both frequency values are in MHz for RadioLib
constexpr float FREQUENCY_435 = 435.00f;
constexpr float FREQUENCY_903 = 903.00f;

static constexpr const char* FREQUENCY_435_STR = "435.00";
static constexpr const char* FREQUENCY_903_STR = "903.00";


// Refer to LoRa Datasheet for allowed bandwidths, in kHz for RadioLib
constexpr float BANDWIDTH_USED = 250.00f;

// Spreading factor (range from 7-12) 
constexpr int SPREADING_FACTOR_USED = 8;

// LoRa Coding Rate Denominator
constexpr int CODING_RATE_USED = 8;

// Sync Word to filter out other LORA tranmissions from other sources
constexpr uint8_t SYNC_WORD = 0x12;

// Output power, in dBm
constexpr int POWER_OUTPUT = 22;

// Length of the LoRa preamble
constexpr int PREAMBLE_LENGTH = 12;

// SW current limit of radio
constexpr float RADIO_CURRENT_LIMIT = 60.0;

// Board-specific constants
constexpr float TCXO_VOLTAGE = 3.0f;
constexpr bool USE_ONLY_LDO = false;
constexpr int RADIO_BUFFER_SIZE = 256;