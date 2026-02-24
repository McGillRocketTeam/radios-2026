#pragma once
#include <stdint.h>

enum class RadioBand : uint8_t { B435, B903 };

struct RadioParams {
    float freq;     // actual tuned frequency (MHz), can be 433.0, 915.0, etc.
    float bw;       // kHz
    int sf;         // spreading factor
    int cr;         // coding rate (your representation)
    int pow;        // TX power
};

// The store will set the initial frequency via the band configured by the frequency pin
// The frequency pin is the pins soldered on the MRT radio hat.
// 435 Band means a sx1262 chip, 900 means a sx1268 chip
// They function extremely similarily but differ on the freq range they handle
// When we start the radio freq equals band, but during runtime the actual freq
// can change, but the band will always stay the same once we set it

namespace ParamStore {
    const RadioParams& get();
    RadioBand band();

    bool is903();
    bool is435();
    //Grabs the compile time default freqs for the band set
    float getDefaultBandFreq();
    // Compares the freq given to the band that has been set
    bool freqAllowed(float f);

    namespace detail {
        void applyFromRadio(const RadioParams& p);
    }
}