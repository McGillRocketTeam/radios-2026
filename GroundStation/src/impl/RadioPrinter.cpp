#include "RadioPrinter.h"
#include "RadioParams.h"
#include "configs/RadioParams.h"
#include "configs/Config.h"
#include <Arduino.h>

namespace RadioPrinter
{
    void radio_params_to_console(const RadioParams *radioParams)
    {
        const size_t TOTAL = PRELIMITER_GS_RADIO_SIZE + sizeof(RadioParams) + DELIMITER_SIZE;
        uint8_t buffer[TOTAL];
        size_t pos = 0;

        memcpy(buffer + pos, PRELIMITER_GS_RADIO, PRELIMITER_GS_RADIO_SIZE);
        pos += PRELIMITER_GS_RADIO_SIZE;

        memcpy(buffer + pos, radioParams, sizeof(RadioParams));
        pos += sizeof(RadioParams);

        memcpy(buffer + pos, DELIMITER, DELIMITER_SIZE);
        pos += DELIMITER_SIZE;

        Serial.write(buffer, pos);
    }
}
