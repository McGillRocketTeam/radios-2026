#pragma once

#if TEENSY == 41

#include <NativeEthernet.h>
#include <fnet.h>

// Static IP of this teensy
inline IPAddress STATIC_IP(192, 168, 0, 50);

// ====== Your PC running the server ========
// inline IPAddress SERVER_IP(192, 168, 0, 142); 
inline IPAddress SERVER_IP(192, 168, 0, 105); 

// Ethernet and MQTT Reconnect check timer in micro seconds 
// ( 1,000,000 micro == 1 second)
inline constexpr int ETHERNET_RECONNECT_INTERVAL = 5 * 1000 * 1000;

// Timout to consider a link to be dead
// make sure its shorter than the reconnect check interval
inline constexpr int TCP_TIMEOUT_SETTING = 4000;

// Teensy MAC 
inline byte MAC[] = { 0x04, 0xE9, 0xE5, 0x12, 0x34, 0x56 };

// IP configs
inline const int SERVER_PORT = 1883;
inline const IPAddress GATEWAY_IP(192, 168, 0, 1);
inline const IPAddress SUBNET_MASK(255, 255, 255, 0);
inline const IPAddress DNS_IP(192, 168, 0, 1);


#endif