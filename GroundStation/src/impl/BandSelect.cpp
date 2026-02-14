#include "BandSelect.h"

#include "Config.h"
#include "PinLayout.h"
#include "LoggerGS.h"

namespace BandSelect
{
    static bool g_inited = false;
    static float g_freq = FREQUENCY_903;

    static void ensureInit()
    {
        if (g_inited)
            return;

        pinMode(FREQ_PIN, INPUT);

        int v = digitalRead(FREQ_PIN);
        g_freq = (v == HIGH) ? FREQUENCY_903 : FREQUENCY_435;

        if (v == HIGH)
        {
            LOGGING(CAT_RADIO ,DEBUG, "FREQ_PIN is HIGH using 900 MHz band");
        }
        else
        {
            LOGGING(CAT_RADIO ,DEBUG, "FREQ_PIN is LOW using 430 MHz band");
        }

        g_inited = true;
    }

    float get()
    {
        ensureInit();
        return g_freq;
    }

    bool is903() { return get() == FREQUENCY_903; }
    bool is435() { return get() == FREQUENCY_435; }

    bool freqAllowedFromBand(float f)
    {
        ensureInit();
        // Allowed from canada ISED amateur HAM bands AND sx1262 limiations
        // Be aware that above 438 we start clashing with satelite associated stuff
        if (g_freq == FREQUENCY_903)
            return (f > 902.0f && f < 928.0f);
        if (g_freq == FREQUENCY_435)
            return (f > 430.0f && f < 440.0f);
        return false;
    }

}
