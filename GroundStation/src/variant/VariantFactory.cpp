#include "IVariant.h"

#include "GroundStationVariant.h"
#include "TestVariant.h"
#include "FlightComputerVariant.h"
#include "RadioChipTestVariant.h"
#include "RangeTestFCVariant.h"
#include "RangeTestGSVariant.h"
#include "TxRxLedTestVariant.h"

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
#elif defined(DEVICE_VARIANT_RANGETEST_FC)
    static RangeTestFCVariant variant;
    return variant;
#elif defined(DEVICE_VARIANT_RANGETEST_GS)
    static RangeTestGSVariant variant;
    return variant;
#elif defined(DEVICE_VARIANT_FLIGHTCOMPUTER)
    static FlightComputerVariant variant;
    return variant;
#elif defined(DEVICE_VARIANT_TXRXLED_TEST)
    static TxRxLedTestVariant variant;
    return variant;
#else
    #error "No valid device mode selected"
#endif
}