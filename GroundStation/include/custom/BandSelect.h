#pragma once
#include <Arduino.h>

namespace BandSelect {
    
    // Read cached frequency (auto-inits on first call)
    float get();

    // Optional: force refresh (also auto-inits)
    void update();

    bool is903();
    bool is435();
}

