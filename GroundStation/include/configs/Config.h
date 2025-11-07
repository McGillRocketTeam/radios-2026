#ifndef CONFIG_H
#define CONFIG_H

// Both frequency values are in MHz for RadioLib
constexpr float FREQUENCY_435 = 435.00f;
constexpr float FREQUENCY_903 = 903.00f;

// Refer to LoRa Datasheet for allowed bandwidths, in kHz for RadioLib
constexpr float BANDWIDTH_USED = 250.00f;

// Spreading factor (range from 7-12)
constexpr int SPREADING_FACTOR_USED = 8;

// LoRa Coding Rate Denominator
constexpr int CODING_RATE_USED = 8;

// Sync Word to filter out other LORA tranmissions from other sources
constexpr uint8_t SYNC_WORD = 0x12;

// Output power, in dBm
constexpr int POWER_OUTPUT = 22;

// Length of the LoRa preamble
constexpr int PREAMBLE_LENGTH = 12;

// Board-specific constants
constexpr float TCXO_VOLTAGE = 3.0f;
constexpr bool USE_ONLY_LDO = false;
constexpr int RADIO_BUFFER_SIZE = 256;

//Ethernet and Serial communication constants
//This is a special debug outside of the logger specific to the console. 
constexpr bool CONSOLE_ROOT_DEBUG = true; 
constexpr int GS_SERIAL_BAUD_RATE = 9600;

// Radio identity and keywording
constexpr const char *RADIO_CALL_SIGN = "VA2 JWL";
constexpr const char *RADIO_COMMAND_KEYWORD = "radio";
constexpr const char *DELIMITER = "<LEO?>";
constexpr size_t DELIMITER_SIZE = 6;
constexpr const char *PRELIMITER_GS_RADIO = "GSRadio";
constexpr size_t PRELIMITER_GS_RADIO_SIZE = 7;
constexpr const char *RADIO_ACK_FOR_GUI = "ack_r";


// PACKET_TO_CMD_RATIO controls how many times we tx a cmd every time we get a CTS
constexpr uint8_t CTS_TO_CMD_RATIO = 1;
constexpr int DELAYS_BETWEEN_REPEATED_CMD = 50;

// Debugging Config
constexpr bool ENABLE_RADIO_TX = true;
constexpr bool ENABLE_RADIO_DEBUG_MODE = true;
constexpr bool ENABLE_PRINT_VERBOSE_PACKET = true;
constexpr bool ENABLE_PRINT_TO_GUI = false;
constexpr bool ENABLE_COMMAND_PARSER_QUEUE = false;
constexpr bool ENABLE_TABBED_BOOLEANS_PACKET_PRINT = false;
constexpr bool ENABLE_GS_PRINT = true;
constexpr bool ENABLE_FC_PACKET_PRINT = false;

const String DEBUG_RADIO = "Radio";
const String DEBUG_VERBOSE_PACKET = "Verbose";
const String DEBUG_GUI_PRINT = "GUI";
const String DEBUG_PARSER = "Parser";
const String DEBUG_BOOLEANS_PACKET = "Booleans_print";
const String DEBUG_GS = "GroundStation";
const String DEBUG_FC_PACKET =  "FC_print";

#endif // CONFIG__H
