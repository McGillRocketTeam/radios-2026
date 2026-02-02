#include "BandSelect.h"

#include "Config.h"
#include "PinLayout.h"
#include "LoggerGS.h"

namespace BandSelect {
    static bool g_inited = false;
    static int g_pin = FREQ_PIN;
    static float g_freq = FREQUENCY_903;

    static void ensureInit() {
        if (g_inited) return;

        pinMode(g_pin, INPUT);

        int v = digitalRead(g_pin);
        g_freq = (v == HIGH) ? FREQUENCY_903 : FREQUENCY_435;

        if (v == HIGH) {
            LOGGING(PIPE, "FREQ_PIN is HIGH using 900 MHz band");
        }
        else{
            LOGGING(PIPE, "FREQ_PIN is LOW using 430 MHz band");
        }

        g_inited = true;
    }

    void update() {
        ensureInit();
        int v = digitalRead(g_pin);
        g_freq = (v == HIGH) ? FREQUENCY_903 : FREQUENCY_435;
    }

    float get() {
        ensureInit();
        return g_freq;
    }

    bool is903() { return get() == FREQUENCY_903; }
    bool is435() { return get() == FREQUENCY_435; }
}
