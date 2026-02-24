#pragma once

#include <cstdint>
#include <Arduino.h>

namespace GSCommand
{
    enum class Command : uint8_t
    {
        Freq,
        Bw,
        Cr,
        Sf,
        Pow,
        Param,
        Ground,
        Ping,
        Status,
        Bypass,
        SetTx,
        Verbose,
        CatGS,
        CatRadio,
        CatParser,
        Unknown
    };

    Command parseRadioCmd(const String &p);

    bool parseBoolTF(const String &v, bool &out);

} // Gs
