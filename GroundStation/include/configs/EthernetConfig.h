#include <NativeEthernet.h>
#include <fnet.h>

// Static IP of this teensy
IPAddress STATIC_IP(192, 168, 0, 50);

// ====== Your PC running the server ========

// IPAddress SERVER_IP(192, 168, 0, 248); 
IPAddress SERVER_IP(192, 168, 0, 105);  

// Ethernet and MQTT Reconnect check timer in micro seconds 
// ( 1,000,000 micro == 1 second)
constexpr int ETHERNET_RECONNECT_INTERVAL = 5 * 1000 * 1000;

// Timout to consider a link to be dead
// make sure its shorter than the reconnect check interval
constexpr int TCP_TIMEOUT_SETTING = 4000;

// Teensy MAC 
byte MAC[] = { 0x04, 0xE9, 0xE5, 0x12, 0x34, 0x56 };

// IP configs
const int SERVER_PORT = 1883;
const IPAddress GATEWAY_IP(192, 168, 0, 1);
const IPAddress SUBNET_MASK(255, 255, 255, 0);
const IPAddress DNS_IP(192, 168, 0, 1);


