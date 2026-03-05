#include "RocketCommand.h"

#include <cstring>
#include <cstdlib>
#include <cstddef>

#include "CommandLine.h"
#include "command_packet.h"

// Hard requirement: args must be aligned if you ever dereference out.data.args[i].
static_assert(offsetof(command_packet_extended_data, args) % 4 == 0,
              "command_packet_extended_data.args is not 4-byte aligned;");

namespace
{
    // Allowed seperators
    inline bool is_id_cmd_sep(char c) { return c == ',' || c == ':'; }
    inline bool is_arg_sep(char c) { return c == ',' || c == ':' || line_util::is_space(c); }

    // Parse uint8 decimal, advances cursor.
    bool parse_u8_dec(const command_line &cmd, uint8_t &cursor, uint8_t end, uint8_t &out)
    {
        cursor = line_util::skip_spaces(cmd, cursor, end);
        if (cursor >= end || !line_util::is_digit(cmd.buf[cursor]))
            return false;

        uint16_t acc = 0;
        while (cursor < end && line_util::is_digit(cmd.buf[cursor]))
        {
            acc = (uint16_t)(acc * 10u + (uint16_t)(cmd.buf[cursor] - '0'));
            if (acc > 255u)
                return false;
            cursor++;
        }
        out = (uint8_t)acc;
        return true;
    }

    bool consume_id_cmd_sep(const command_line &cmd, uint8_t &cursor, uint8_t end)
    {
        cursor = line_util::skip_spaces(cmd, cursor, end);
        if (cursor >= end || !is_id_cmd_sep(cmd.buf[cursor]))
            return false;
        cursor++;
        return true;
    }

    // Parse cmd token span (1..5 chars), stops at comma/space/end.
    bool parse_cmd_span(const command_line &cmd, uint8_t &cursor, uint8_t end,
                        uint8_t &s, uint8_t &t)
    {
        cursor = line_util::skip_spaces(cmd, cursor, end);
        if (cursor >= end)
            return false;

        s = cursor;
        uint8_t n = 0;
        while (cursor < end && !is_arg_sep(cmd.buf[cursor]))
        {
            if (n >= 5)
                return false;
            n++;
            cursor++;
        }
        t = cursor;
        return t > s;
    }

    bool copy_cmd_to_cstr6(const command_line &cmd, uint8_t s, uint8_t t, char out6[6])
    {
        uint8_t n = (uint8_t)(t - s);
        if (n == 0 || n > 5)
            return false;
        for (uint8_t i = 0; i < n; i++)
            out6[i] = cmd.buf[s + i];
        out6[n] = '\0';
        return true;
    }

    // Parse arg spans after cmd: comma/space separated.
    uint8_t parse_arg_spans(const command_line &cmd, uint8_t &cursor, uint8_t end,
                            uint8_t *starts, uint8_t *ends, uint8_t maxArgs)
    {
        uint8_t argc = 0;
        while (argc < maxArgs)
        {
            while (cursor < end && is_arg_sep(cmd.buf[cursor]))
                cursor++;
            if (cursor >= end)
                break;

            uint8_t s = cursor;
            while (cursor < end && !is_arg_sep(cmd.buf[cursor]))
                cursor++;
            uint8_t t = cursor;

            if (t > s)
            {
                starts[argc] = s;
                ends[argc] = t;
                argc++;
            }
        }
        return argc;
    }

    bool parse_bool_tf_span(const command_line &cmd, uint8_t s, uint8_t t, bool &out)
    {
        if ((t - s) != 1)
            return false;
        char c = line_util::to_lower_ascii(cmd.buf[s]);
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

    bool parse_float_span_or_tf(const command_line &cmd, uint8_t s, uint8_t t, float &out)
    {
        // handle t/f
        bool b;
        if (parse_bool_tf_span(cmd, s, t, b))
        {
            out = b ? 1.0f : 0.0f;
            return true;
        }

        // copy into small temp C-string for strtof
        const uint8_t n = (uint8_t)(t - s);
        if (n == 0 || n >= 32)
            return false; // keep it safe and bounded
        char tmp[32];
        for (uint8_t i = 0; i < n; i++)
            tmp[i] = cmd.buf[s + i];
        tmp[n] = '\0';

        char *endp = nullptr;
        float f = strtof(tmp, &endp);
        if (endp == tmp || *endp != '\0')
            return false;

        out = f;
        return true;
    }

    /// For debug print
    float safe_read_float(const command_packet_extended &pkt, uint8_t i)
    {
        float f = 0.0f;
        // Read from raw bytes to avoid misaligned float access in packed structs
        const uint8_t *base = pkt.bytes;

        // Compute offset to args[i] inside the byte array
        // This is safe even if the struct is packed.
        const size_t off = offsetof(command_packet_extended_data, args) + (size_t)i * sizeof(float);

        std::memcpy(&f, base + off, sizeof(float));
        return f;
    }

} // anonymous namespace

bool RocketCommand::fillCommandPacket(const command_line &line, command_packet_extended &out)
{
    std::memset(out.bytes, 0, sizeof(out.bytes));

    uint8_t start, end;
    line_util::trimmed_span(line, start, end);

    uint8_t cur = start;

    // id
    uint8_t id = 0;
    if (!parse_u8_dec(line, cur, end, id))
        return false;

    // sep
    if (!consume_id_cmd_sep(line, cur, end))
        return false;

    // cmd span -> command_string[6]
    uint8_t cs, ct;
    if (!parse_cmd_span(line, cur, end, cs, ct))
        return false;

    char cmdstr[6];
    if (!copy_cmd_to_cstr6(line, cs, ct, cmdstr))
        return false;

    out.data.base.data.command_id = id;
    std::strncpy(out.data.base.data.command_string, cmdstr, 5);
    out.data.base.data.command_string[5] = '\0';

    // args (up to 3)
    uint8_t aS[3], aE[3];
    uint8_t argc = parse_arg_spans(line, cur, end, aS, aE, 3);
    out.data.argc = argc;

    for (uint8_t i = 0; i < argc; i++)
    {
        float f = 0.0f;
        if (!parse_float_span_or_tf(line, aS[i], aE[i], f))
            return false;
        out.data.args[i] = f;
    }

    return true;
}

void RocketCommand::printCommandPacket(const command_packet_extended &pkt, Print &out)
{
    out.print("command_id=");
    out.print(pkt.data.base.data.command_id);

    out.print(" command_string=\"");
    char s[6];
    std::memcpy(s, pkt.data.base.data.command_string, 6);
    s[5] = '\0';
    out.print(s);
    out.print("\"");

    out.print(" argc=");
    uint8_t argc = pkt.data.argc;
    out.print(argc);

    out.print(" args=[");
    if (argc > 3)
        argc = 3;

    for (uint8_t i = 0; i < argc; i++)
    {
        if (i)
            out.print(", ");
        out.print(safe_read_float(pkt, i), 2); 
    }
    out.println("]");
}

bool RocketCommand::tryParseCommandId(const command_line& line, uint8_t& outId)
{
    uint8_t start, end;
    line_util::trimmed_span(line, start, end);

    uint8_t cur = start;
    return parse_u8_dec(line, cur, end, outId);
}