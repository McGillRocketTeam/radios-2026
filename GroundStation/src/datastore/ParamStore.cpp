#include "ParamStore.h"

#include "Config.h"
#include "PinLayout.h"
#include "LoraParamConfig.h"
#include "LoggerGS.h"

namespace ParamStore
{
    static bool g_inited = false;
    static RadioBand g_band = RadioBand::B435;

    static RadioParams g_params = {
        FREQUENCY_435,
        BANDWIDTH_USED,
        SPREADING_FACTOR_USED,
        CODING_RATE_USED,
        POWER_OUTPUT};

    static RadioParams defaultsFor435()
    {
        return {FREQUENCY_435, BANDWIDTH_USED, SPREADING_FACTOR_USED, CODING_RATE_USED, POWER_OUTPUT};
    }

    static RadioParams defaultsFor903()
    {
        return {FREQUENCY_903, BANDWIDTH_USED, SPREADING_FACTOR_USED, CODING_RATE_USED, POWER_OUTPUT};
    }

    static void ensureInit()
    {
        if (g_inited)
            return;

        pinMode(FREQ_PIN, INPUT);
        delay(500);
        // TO DO fix this analog read once the RF_SW pin is fixed
        int v = analogRead(FREQ_PIN);

        if (v > 100)
        {
            g_band = RadioBand::B903;
            g_params = defaultsFor903();
            LOGGING(CAT_RADIO, DEBUG, "FREQ_PIN is HIGH using 900 MHz band");
        }
        else
        {
            g_band = RadioBand::B435;
            g_params = defaultsFor435();
            LOGGING(CAT_RADIO, DEBUG, "FREQ_PIN is LOW using 430 MHz band");
        }

        g_inited = true;
    }

    RadioBand band()
    {
        ensureInit();
        return g_band;
    }

    const RadioParams &get()
    {
        ensureInit();
        return g_params;
    }

    bool is903() { return band() == RadioBand::B903; }
    bool is435() { return band() == RadioBand::B435; }

    bool freqAllowed(float f)
    {
        ensureInit();
        if (g_band == RadioBand::B903)
            return (f > 902.0f && f < 928.0f);
        if (g_band == RadioBand::B435)
            return (f > 430.0f && f < 440.0f);
        return false;
    }
     
    float getDefaultBandFreq()
    {
        ensureInit();
        if (g_band == RadioBand::B435)
        {
            return FREQUENCY_435;
        }
        else
        {
            return FREQUENCY_903;
        }
    }

    namespace detail
    {
        // Make sure the freq is valid before sending the params
        void applyFromRadio(const RadioParams &p)
        {
            ensureInit();
            g_params = p;
        }
    }
}