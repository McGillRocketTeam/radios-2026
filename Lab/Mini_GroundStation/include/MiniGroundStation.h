#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

#include "ButtonPad.h"
#include "RadioReceiver.h"
#include "TelemetryStore.h"
#include "peripheralPins.h"

class MiniGroundStation
{
public:
    void setup();
    void poll();

private:
    enum class PageMode : uint8_t
    {
        INTRO,
        TELEMETRY
    };

    static constexpr uint32_t RADIO_RETRY_INTERVAL_MS = 1000;

    void pollRadio();
    void pollButtons();
    void handleButtonEvent(ButtonPad::Event event);
    void redrawCurrentPage();
    void drawIntroPage();

    U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI display_{
        U8G2_R0,
        DISPLAY_CS_PIN,
        DISPLAY_DC_PIN,
        DISPLAY_RST_PIN};

    ButtonPad buttonPad_{
        BUTTON_PAD_LEFT_PIN,
        BUTTON_PAD_RIGHT_PIN,
        BUTTON_PAD_UP_PIN,
        BUTTON_PAD_DOWN_PIN,
        BUTTON_PAD_PRESSED_PIN};

    RadioReceiver radio_;
    TelemetryStore telemetry_;

    PageMode currentPage_ = PageMode::TELEMETRY;
    uint8_t displayOffset_ = 0;
    uint32_t displayedTelemetryRevision_ = 0;
    uint32_t lastRadioInitAttemptMs_ = 0;
};
