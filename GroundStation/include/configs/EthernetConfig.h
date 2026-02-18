#pragma once
#if TEENSY == 41

#include <QNEthernet.h>
using namespace qindesign::network;

// Posible IP addresses of computers running the MQTT server
static const IPAddress kServers[] = {
    IPAddress(192, 168, 0, 142),
    IPAddress(192, 168, 0, 105)
};
static constexpr size_t kNumServers = sizeof(kServers) / sizeof(kServers[0]);

static size_t s_serverIdx = 0;

inline constexpr int ETHERNET_RECONNECT_INTERVAL = 10 * 1000 * 1000;
inline constexpr int DHCP_IP_ASSIGN_TIMEOUT = 2000;
inline constexpr int INITIAL_PHY_LINK_TIMEOUT = 1000;
inline const int SERVER_PORT = 1883;

#endif
