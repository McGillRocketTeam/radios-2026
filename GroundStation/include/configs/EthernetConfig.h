#pragma once
#if TEENSY == 41

#include <QNEthernet.h>
using namespace qindesign::network;

// Interval between etherent and mqtt reconnect attempts
inline constexpr int ETHERNET_RECONNECT_INTERVAL = 10 * 1000 * 1000;


inline constexpr int DHCP_IP_ASSIGN_TIMEOUT = 2000;
inline constexpr int INITIAL_PHY_LINK_TIMEOUT = 1000;

//Broker host name, needs to be set in the router's DNS to point
// to the router assigned ip of the laptop running the broker
inline constexpr const char *BROKER_HOST = "mrt_mqtt";
inline const int SERVER_PORT = 1883;

#endif
