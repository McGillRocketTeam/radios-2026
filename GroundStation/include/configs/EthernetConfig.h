
#if TEENSY_BOARD_VERSION == 41
#include <NativeEthernet.h>
#include <fnet.h>

byte MAC[] = { 0x04, 0xE9, 0xE5, 0x12, 0x34, 0x56 };
IPAddress STATIC_IP(192, 168, 8, 50);   // Teensy's static IP
// IPAddress SERVER_IP(192, 168, 8, 248);  // Your PC running the server
IPAddress SERVER_IP(192, 168, 8, 107);  // Your PC running the server
const int SERVER_PORT = 9000;

// Defaults
const IPAddress GATEWAY_IP(192, 168, 8, 1);
const IPAddress SUBNET_MASK(255, 255, 255, 0);
const IPAddress DNS_IP(192, 168, 8, 1);

#endif
