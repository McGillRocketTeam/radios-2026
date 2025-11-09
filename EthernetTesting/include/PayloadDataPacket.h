#ifndef PAYLOADPACKET_H
#define PAYLOADPACKET_H

#include <cstdint>

typedef struct __attribute__((__packed__)) payload_data
{
    uint16_t pl_vibration_freq_x;         	// Vibration frequency x
    uint16_t pl_vibration_freq_y;         	// Vibration frequency y
    uint16_t pl_vibration_freq_z;         	// Vibration frequency z
    uint16_t pl_vibration_amplitude_x;    	// Vibration amplitude x
    uint16_t pl_vibration_amplitude_y;    	// Vibration amplitude y
    uint16_t pl_vibration_amplitude_z;    	// Vibration amplitude z
    uint32_t pl_time_since_start;         	// Time since start of sampling
    uint8_t pl_target_temp;                 	// Target temperature
    uint16_t pl_sample_temp;              	// Actual sample temperature
    bool pl_muon_state : 1;                  // Muon detector state
    bool pl_sampling_state : 1;              // Sampling state
    bool pl_active_temp_ctrl_state : 1;      // Active temperature control state
} payload_data;

typedef union payload_packet{
  payload_data data;
  uint8_t bytes[sizeof(payload_data)];
  } payload_packet;

#endif //PAYLOADPACKET_H
