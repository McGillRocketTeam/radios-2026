#include "GroundStationStore.h"

namespace GroundStationStore
{
    static bool g_canTxFromCTS = false;

    bool canTxFromCTS()
    {
        return g_canTxFromCTS;
    }

    void setCanTxFromCTS(bool value)
    {
        g_canTxFromCTS = value;
    }
}