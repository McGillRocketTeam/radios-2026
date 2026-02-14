#pragma once
#include <Arduino.h>

// Band select singleton util grabs the band configured by the frequency pin
// The frequency pin is the pins soldered on the MRT radio hat.
// 435 Band means a sx1262 chip, 900 means a sx1268 chip
// They function extremely similarily but differ on the freq range they handle
namespace BandSelect
{

    // Read cached frequency (auto-inits on first call)
    float get();

    bool is903();
    bool is435();

    // Check if freq input matches the amateur ISED frequencies of that band
    bool freqAllowedFromBand(float f);
}
