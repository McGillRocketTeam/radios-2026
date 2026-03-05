#include "CommandLine.h"
#include <cstring>

// Holds all the handy ascii functions related to dealing with command line
namespace line_util
{
    bool is_space(char c) {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }

    bool is_digit(char c)
    {
        return c >= '0' && c <= '9';
    }

    char to_lower_ascii(char c) {
        if (c >= 'A' && c <= 'Z') return (char)(c - 'A' + 'a');
        return c;
    }

    void trimmed_span(const command_line& cmd, uint8_t& start, uint8_t& end) {
        start = 0;
        end = cmd.len;
        while (start < end && is_space(cmd.buf[start])) start++;
        while (end > start && is_space(cmd.buf[end - 1])) end--;
    }

    bool span_ieq_lit(const command_line& cmd, uint8_t start, uint8_t end, const char* lit) {
        const size_t lit_len = std::strlen(lit);
        if ((size_t)(end - start) != lit_len) return false;

        for (size_t i = 0; i < lit_len; i++) {
            if (to_lower_ascii(cmd.buf[start + (uint8_t)i]) != to_lower_ascii(lit[i]))
                return false;
        }
        return true;
    }

    uint8_t find_space(const command_line& cmd, uint8_t start, uint8_t end) {
        for (uint8_t i = start; i < end; i++) {
            if (cmd.buf[i] == ' ') return i;
        }
        return end;
    }

    uint8_t skip_spaces(const command_line& cmd, uint8_t i, uint8_t end) {
        while (i < end && cmd.buf[i] == ' ') i++;
        return i;
    }

    bool next_token(const command_line& cmd, uint8_t& cursor, uint8_t end,
                    uint8_t& tokStart, uint8_t& tokEnd)
    {
        cursor = skip_spaces(cmd, cursor, end);
        if (cursor >= end) return false;

        tokStart = cursor;
        tokEnd = find_space(cmd, tokStart, end);
        cursor = tokEnd;
        return true;
    }
}