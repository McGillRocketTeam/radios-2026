#include "IVariant.h"

#include "GroundStationVariant.h"
#include "TestVariant.h"
#include "FlightComputerVariant.h"
#include "RadioChipTestVariant.h"


IVariant& getVariant()
{
#ifdef DEVICE_VARIANT_TESTING
    static TestVariant variant;
    return variant;
#elif defined(DEVICE_VARIANT_RADIO_TEST)
    static RadioChipTestVariant variant;
    return variant;
#elif defined(DEVICE_VARIANT_GROUNDSTATION)
    static GroundStationVariant variant;
    return variant;
#elif defined(DEVICE_VARIANT_FLIGHTCOMPUTER)
    static FlightComputerVariant variant;
    return variant;
#else
    #error "No valid device mode selected"
#endif
}