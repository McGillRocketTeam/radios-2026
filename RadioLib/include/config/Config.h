#ifndef CONFIG_H
#define CONFIG_H

// Frequency value in MHz
constexpr float FREQUENCY = 903.0f;

// Refer to LoRa Datasheet for allowed bandwidths, in kHz for RadioLib
constexpr float BANDWIDTH = 250.00f;

// Spreading factor (range from 7-12)
constexpr int SPREADING_FACTOR = 8;

// Output power, in dBm
constexpr int POWER_OUTPUT = 22;

#endif