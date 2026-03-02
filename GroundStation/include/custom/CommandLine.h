#pragma once

#include <cstdint>

/**
 * @struct command_line
 * @brief Standardised struct for passing around string inputs 
 */

// Maximum length including null terminator for any single command line.
// 
constexpr uint16_t MAX_COMMAND_LENGTH = 40;
static_assert(MAX_COMMAND_LENGTH < 128, "Our command length can not overflow len");

// All string inputs should be passed around with command_line
typedef struct command_line
{
    uint8_t len;
    char buf[MAX_COMMAND_LENGTH];
} command_line;

namespace line_util
{
    bool is_space(char c);
    bool is_digit(char c);
    char to_lower_ascii(char c);

    // Returns trimmed span [start, end)
    void trimmed_span(const command_line& cmd, uint8_t& start, uint8_t& end);

    // Case-insensitive compare of span to literal ASCII
    bool span_ieq_lit(const command_line& cmd, uint8_t start, uint8_t end, const char* lit);

    // Tokenization helpers (space-delimited by ' ')
    uint8_t find_space(const command_line& cmd, uint8_t start, uint8_t end);
    uint8_t skip_spaces(const command_line& cmd, uint8_t i, uint8_t end);

    bool next_token(const command_line& cmd,
                    uint8_t& cursor,
                    uint8_t end,
                    uint8_t& tokStart,
                    uint8_t& tokEnd);
}