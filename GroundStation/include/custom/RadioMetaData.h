#include <cstdint>

// ---------- radio metadata astra atomic ----------
typedef struct __attribute__((__packed__)) radio_metadata_data
{
    uint8_t radio_rssi;
    int8_t radio_snr; 
} radio_metadata_data;

typedef union radio_metadata_data_packet {
    radio_metadata_data data;
    uint8_t bytes[sizeof(radio_metadata_data)];
} radio_metadata_data_packet;
