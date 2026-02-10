#pragma once

#if TEENSY == 41

#include <NativeEthernet.h>
#include <fnet.h>

// ====== Your PC running the server ========
// inline IPAddress SERVER_IP(192, 168, 0, 142); 
inline IPAddress SERVER_IP(192, 168, 0, 105); 

// Ethernet and MQTT Reconnect check timer in micro seconds 
// ( 1,000,000 micro == 1 second)
inline constexpr int ETHERNET_RECONNECT_INTERVAL = 5 * 1000 * 1000;

// Timout to consider a link to be dead
// make sure its shorter than the reconnect check interval
inline constexpr int TCP_TIMEOUT_SETTING = 4000;

// IP configs
inline const int SERVER_PORT = 1883;

uint8_t* getTeensyMac();

#endif