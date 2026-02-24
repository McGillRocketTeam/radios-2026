#include <cstdint>

// ---------- radio metadata astra atomic ----------
typedef struct __attribute__((__packed__)) radio_metadata_data
{
    // 2B sequence number for the corresponding telemetry frame 
    // these measurements were collected for.
    uint16_t seq;
    
    // values in these fields are raw register values
    uint8_t radio_rssi; // real value is radio_rssi/ -2.0
    int8_t radio_snr;  // real value is radio_snr/4
} radio_metadata_data;

typedef union radio_metadata_data_packet {
    radio_metadata_data data;
    uint8_t bytes[sizeof(radio_metadata_data)];
} radio_metadata_data_packet;
