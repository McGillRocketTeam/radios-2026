#include "drawTelemetry.h"


void formatTelemetryLine(
    const TelemetryInfo& info,
    const TelemetryFieldDescriptor& desc,
    char* out,
    size_t outSize)
{
    const uint8_t* base = reinterpret_cast<const uint8_t*>(&info);
    const void* fieldPtr = base + desc.offset;

    switch (desc.type)
    {
        case FieldType::FLOAT:
        {
            float value = *reinterpret_cast<const float*>(fieldPtr);
            snprintf(out, outSize, "%s:%.*f%s",
                     desc.label,
                     desc.decimals,
                     value,
                     desc.unit);
            break;
        }

        case FieldType::UINT8:
        {
            uint8_t value = *reinterpret_cast<const uint8_t*>(fieldPtr);
            snprintf(out, outSize, "%s:%u%s",
                     desc.label,
                     static_cast<unsigned>(value),
                     desc.unit);
            break;
        }

        case FieldType::UINT16:
        {
            uint16_t value = *reinterpret_cast<const uint16_t*>(fieldPtr);
            snprintf(out, outSize, "%s:%u%s",
                     desc.label,
                     static_cast<unsigned>(value),
                     desc.unit);
            break;
        }

        case FieldType::INT8:
        {
            int8_t value = *reinterpret_cast<const int8_t*>(fieldPtr);
            snprintf(out, outSize, "%s:%d%s",
                     desc.label,
                     static_cast<int>(value),
                     desc.unit);
            break;
        }

        default:
        {
            snprintf(out, outSize, "%s:?", desc.label);
            break;
        }
    }
}

void drawTelemetryPage(U8G2& display, const TelemetryInfo& info, uint8_t startIndex)
{
    display.clearBuffer();
    display.setFont(u8g2_font_6x12_tr);

    const int lineHeight = 12;
    const int maxLines = 5;

    char lineBuf[32];

    for (int i = 0; i < maxLines; i++)
    {
        size_t fieldIndex = startIndex + i;
        if (fieldIndex >= TELEMETRY_FIELD_COUNT)
        {
            break;
        }

        formatTelemetryLine(info, TELEMETRY_FIELDS[fieldIndex], lineBuf, sizeof(lineBuf));
        display.drawStr(0, (i + 1) * lineHeight, lineBuf);
    }

    display.sendBuffer();
}