#pragma once

#include <cstdint>
#include <Arduino.h>

#include "CommandLine.h"

namespace GroundCommand
{

    enum class Action : uint8_t
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

    struct Cmd
    {
        float arg;
        Action action;
    };

    bool isGroundCommand(const command_line& s);
    Action parseActionToken(const command_line& cmd,uint8_t start, uint8_t end);
    Cmd parseGroundCmd(const command_line &s);

} // namespace GSCommand