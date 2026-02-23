#include "GSCommand.h"

GSCommand::Command GSCommand::parseRadioCmd(const String &p)
{
    if (p == "freq")
        return Command::Freq;
    if (p == "bw")
        return Command::Bw;
    if (p == "cr")
        return Command::Cr;
    if (p == "sf")
        return Command::Sf;
    if (p == "pow")
        return Command::Pow;
    if (p == "param")
        return Command::Param;
    if (p == "ground")
        return Command::Ground;
    if (p == "ping")
        return Command::Ping;
    if (p == "init")
        return Command::Init;
    if (p == "bypass")
        return Command::Bypass;
    if (p == "settx")
        return Command::SetTx;
    if (p == "verbose")
        return Command::Verbose;
    if (p == "catgs")
        return Command::CatGS;
    if (p == "catradio")
        return Command::CatRadio;
    if (p == "catparser")
        return Command::CatParser;
    return Command::Unknown;
}

bool GSCommand::parseBoolTF(const String &v, bool &out)
{
    if (v == "t")
    {
        out = true;
        return true;
    }
    if (v == "f")
    {
        out = false;
        return true;
    }
    return false;
}