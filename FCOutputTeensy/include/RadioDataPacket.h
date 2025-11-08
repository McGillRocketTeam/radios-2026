#ifndef MRT_RADIO_PACKET_H
#define MRT_RADIO_PACKET_H

#include <cstdint>

typedef struct __attribute__((__packed__)) radio_data
{
	uint8_t CTSFlag; 				// A single byte variable for CTS flag, either 'y' or 'n'
	uint8_t packet_id;					// 32 bits to store the packet number of transmission
	uint8_t HAM_call_sign[6];        	// HAM call sign (48 bits)
} radio_data;

typedef union radio_packet
{
  radio_data data;
  uint8_t bytes[sizeof(radio_data)];
} radio_packet;

#endif /* MRT_RADIO_PACKET_H */
