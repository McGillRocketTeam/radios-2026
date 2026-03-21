#include "GroundCommand.h"

#include <cstdlib>
#include <cctype>
#include <cstdint>
#include <cstring>

#include "CommandLine.h"
#include "Config.h"

namespace
{
    bool isRawPing(const command_line &cmd)
    {
        uint8_t a, b;
        line_util::trimmed_span(cmd, a, b);
        return line_util::span_ieq_lit(cmd, a, b, "ping"); // case-insensitive
    }

    // Extract token span [tokStart, tokEnd) from within [start,end)
    // Token ends at space or end.
    bool next_token(const command_line &cmd, uint8_t &cursor, uint8_t end,
                    uint8_t &tokStart, uint8_t &tokEnd)
    {
        cursor = line_util::skip_spaces(cmd, cursor, end);
        if (cursor >= end)
            return false;

        tokStart = cursor;
        uint8_t sp = line_util::find_space(cmd, tokStart, end);
        tokEnd = sp;
        cursor = sp;
        return true;
    }

    // Convert a span to a temporary null-terminated buffer for strtof
    bool span_to_cstr(const command_line &cmd, uint8_t start, uint8_t end, char *out, size_t outCap)
    {
        size_t n = (size_t)(end - start);
        if (n + 1 > outCap)
            return false;
        for (size_t i = 0; i < n; i++)
            out[i] = cmd.buf[start + (uint8_t)i];
        out[n] = '\0';
        return true;
    }

    bool parseBoolTF(char c, bool &out)
    {
        c = line_util::to_lower_ascii(c);
        if (c == 't')
        {
            out = true;
            return true;
        }
        if (c == 'f')
        {
            out = false;
            return true;
        }
        return false;
    }

    float parseArgSpanToFloatOrZero(const command_line &cmd, uint8_t start, uint8_t end)
    {
        // Trim outer whitespace
        while (start < end && line_util::is_space(cmd.buf[start]))
            start++;
        while (end > start && line_util::is_space(cmd.buf[end - 1]))
            end--;

        if (start >= end)
            return 0.0f;

        if ((end - start) == 1)
        {
            bool b;
            if (parseBoolTF(cmd.buf[start], b))
                return b ? 1.0f : 0.0f;
        }

        char tmp[MAX_COMMAND_LENGTH];
        if (!span_to_cstr(cmd, start, end, tmp, sizeof(tmp)))
            return 0.0f;

        char *endp = nullptr;
        const float f = strtof(tmp, &endp);
        if (endp != tmp && *endp == '\0')
            return f;
        return 0.0f;
    }

} // namespace

bool GroundCommand::isGroundCommand(const command_line &cmd)
{
    if (isRawPing(cmd))
        return true;

    // Must be: "<RADIO_COMMAND_KEYWORD><space>..."
    if (cmd.len <= RADIO_COMMAND_KEY_LEN)
        return false;

    for (uint16_t i = 0; i < RADIO_COMMAND_KEY_LEN; i++)
    {
        if (cmd.buf[i] != RADIO_COMMAND_KEYWORD[i])
            return false;
    }
    return cmd.buf[RADIO_COMMAND_KEY_LEN] == ' ';
}

GroundCommand::Action GroundCommand::parseActionToken(const command_line &cmd,
                                                      uint8_t start, uint8_t end)
{
    if (line_util::span_ieq_lit(cmd, start, end, "freq"))
        return Action::Freq;
    if (line_util::span_ieq_lit(cmd, start, end, "bw"))
        return Action::Bw;
    if (line_util::span_ieq_lit(cmd, start, end, "cr"))
        return Action::Cr;
    if (line_util::span_ieq_lit(cmd, start, end, "sf"))
        return Action::Sf;
    if (line_util::span_ieq_lit(cmd, start, end, "pow"))
        return Action::Pow;
    if (line_util::span_ieq_lit(cmd, start, end, "param"))
        return Action::Param;
    if (line_util::span_ieq_lit(cmd, start, end, "ground"))
        return Action::Ground;
    if (line_util::span_ieq_lit(cmd, start, end, "ping"))
        return Action::Ping;
    if (line_util::span_ieq_lit(cmd, start, end, "status"))
        return Action::SetStatus;
    if (line_util::span_ieq_lit(cmd, start, end, "bypass"))
        return Action::Bypass;
    if (line_util::span_ieq_lit(cmd, start, end, "settx"))
        return Action::SetTx;
    if (line_util::span_ieq_lit(cmd, start, end, "verbose"))
        return Action::Verbose;
    if (line_util::span_ieq_lit(cmd, start, end, "catgs"))
        return Action::CatGS;
    if (line_util::span_ieq_lit(cmd, start, end, "catradio"))
        return Action::CatRadio;
    if (line_util::span_ieq_lit(cmd, start, end, "catparser"))
        return Action::CatParser;
    if (line_util::span_ieq_lit(cmd, start, end, "clear"))
        return Action::Clear;
    return Action::Unknown;
}

GroundCommand::Cmd GroundCommand::parseGroundCmd(const command_line &cmdline)
{
    Cmd out{0.0f, Action::Unknown};

    if (isRawPing(cmdline))
    {
        out.action = Action::Ping;
        out.arg = 0.0f;
        return out;
    }

    if (!isGroundCommand(cmdline))
        return out;

    uint8_t lineStart = 0, lineEnd = 0;
    line_util::trimmed_span(cmdline, lineStart, lineEnd);

    uint8_t cursor = RADIO_COMMAND_KEY_LEN;
    uint8_t a0, a1;
    if (!next_token(cmdline, cursor, lineEnd, a0, a1))
        return out;
    out.action = GroundCommand::parseActionToken(cmdline, a0, a1);

    cursor = line_util::skip_spaces(cmdline, cursor, lineEnd);
    if (cursor < lineEnd)
    {
        // arg is [cursor, lineEnd)
        out.arg = parseArgSpanToFloatOrZero(cmdline, cursor, lineEnd);
    }
    else
    {
        out.arg = 0.0f;
    }

    return out;
}