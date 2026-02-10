#if TEENSY==41 && defined(TEENSYDUINO) && defined(__IMXRT1062__)


#include <Arduino.h>
#include <NativeEthernet.h>

#include "imxrt.h"   // ensures HW_OCOTP_MAC0/MAC1 are declared
#include "EthernetConfig.h"

uint8_t* getTeensyMac()
{
    static uint8_t mac[6];
    static bool inited = false;

    if (!inited)
    {
        const uint32_t m1 = HW_OCOTP_MAC1;
        const uint32_t m0 = HW_OCOTP_MAC0;

        mac[0] = (uint8_t)(m1 >> 8);
        mac[1] = (uint8_t)(m1 >> 0);
        mac[2] = (uint8_t)(m0 >> 24);
        mac[3] = (uint8_t)(m0 >> 16);
        mac[4] = (uint8_t)(m0 >> 8);
        mac[5] = (uint8_t)(m0 >> 0);

        inited = true;
    }
    return mac;
}

#endif