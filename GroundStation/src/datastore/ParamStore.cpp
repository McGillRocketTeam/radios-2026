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
        delay(400);
        // TO DO fix this analog read once the RF_SW pin is fixed
        int v = analogRead(FREQ_PIN);
        char debug[64];
        if (v > 50)
        {
            g_band = RadioBand::B903;
            g_params = defaultsFor903();
            snprintf(debug, sizeof(debug), "FREQ_PIN is HIGH at value %d using 900 MHz band", v);
            LOGGING(CAT_RADIO, DEBUG, debug);
        }
        else
        {
            g_band = RadioBand::B435;
            g_params = defaultsFor435();
            snprintf(debug, sizeof(debug), "FREQ_PIN is LOW at value %d using 433 MHz band", v);
            LOGGING(CAT_RADIO, DEBUG, debug);
        }

        g_inited = true;
    }

    RadioBand band()
    {
        ensureInit();
        return g_band;
    }

    const RadioParams &getRadioParams()
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

    int getFreqPinAnalogValue()
    {
        return analogRead(FREQ_PIN);
    }

    void applyParamsToCurrentBand(const RadioParams &input)
    {
        ensureInit();

        RadioParams updated = input;
        if (!freqAllowed(updated.freq))
        {
            LOGGING(CAT_RADIO,CRIT,"set a new param to current band with bad frequency");
            updated.freq = getDefaultBandFreq();
        }

        g_params = updated;
    }
}