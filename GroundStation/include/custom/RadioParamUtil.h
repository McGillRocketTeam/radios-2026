#pragma once

#include <cmath>    
#include <cstdint> 

// Converts GS LoRa BW MHz human readable format to rocket enum mapping
inline uint8_t toLoraBwEnum(float bw_khz) {
    auto near = [](float a, float b){ return fabsf(a-b) < 0.6f; };
    if (near(bw_khz, 500.0f))  return 6;   // SX126X_LORA_BW_500
    if (near(bw_khz, 250.0f))  return 5;   // SX126X_LORA_BW_250
    if (near(bw_khz, 125.0f))  return 4;   // SX126X_LORA_BW_125
    if (near(bw_khz, 62.5f))   return 3;   // SX126X_LORA_BW_062
    if (near(bw_khz, 41.7f))   return 10;  // SX126X_LORA_BW_041
    if (near(bw_khz, 31.25f))  return 2;   // SX126X_LORA_BW_031
    if (near(bw_khz, 20.8f))   return 9;   // SX126X_LORA_BW_020
    if (near(bw_khz, 15.6f))   return 1;   // SX126X_LORA_BW_015
    if (near(bw_khz, 10.4f))   return 8;   // SX126X_LORA_BW_010
    if (near(bw_khz, 7.8f))    return 0;   // SX126X_LORA_BW_007
    // fallback
    return 3; // default to 62.5 kHz
}

// Converts LoRa CR GS human readable format to rocket enum format
inline uint8_t toLoraCrEnum(int cr_human) {
    if (cr_human >= 1 && cr_human <= 4) return (uint8_t)cr_human;
    switch (cr_human) {
        case 5: return 1; // 4/5
        case 6: return 2; // 4/6
        case 7: return 3; // 4/7
        case 8: return 4; // 4/8
        default: return 4; // default to 4/8
    }
}
