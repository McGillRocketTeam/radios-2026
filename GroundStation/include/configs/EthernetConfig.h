#pragma once

#if TEENSY == 41

#include <NativeEthernet.h>
#include <fnet.h>

// ====== Ip list for possbile PCs running the server ========
static const IPAddress kServers[] = {
    IPAddress(192, 168, 0, 142),
    IPAddress(192, 168, 0, 105)
};
static constexpr size_t kNumServers = sizeof(kServers) / sizeof(kServers[0]);

// Offset to access the last succesful server connection
static size_t s_serverIdx = 0;

// Ethernet and MQTT Reconnect check timer in micro seconds 
// ( 1,000,000 micro == 1 second)
inline constexpr int ETHERNET_RECONNECT_INTERVAL = 10 * 1000 * 1000;

// Total Timout to consider a link to be dead for dhcp
inline constexpr int ETHERNET_TOTAL_TIMEOUT = 2000;

// Timeout per dhcp try, make sure its less than total 
inline constexpr int ETHERNET_RESPONSE_TIMOUT = 500; 

// IP configs
inline const int SERVER_PORT = 1883;

uint8_t* getTeensyMac();

#endif