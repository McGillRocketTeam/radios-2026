#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

#include "ButtonPad.h"
#include "MockFlightComputer.h"
#include "RadioReceiver.h"
#include "TelemetryStore.h"
#include "peripheralPins.h"

class MiniGroundStation
{
public:
    void setup();
    void poll();

private:

    enum class Screen : uint8_t
    {
        MODE,
        MENU,
        LAUNCH,
        INTRO
    };

    enum class OperatingMode : uint8_t
    {
        RECOVERY,
        MOCKFC
    };

    static constexpr uint32_t RADIO_RETRY_INTERVAL_MS = 1000;
    static constexpr uint32_t PAGE_NAME_DURATION_MS = 800;
    static constexpr uint32_t LAUNCH_DURATION_MS = 900;
    static constexpr uint32_t LAUNCH_FRAME_INTERVAL_MS = 33;

    void pollRadio();
    void pollButtons();
    void handleButtonEvent(ButtonPad::Event event);
    void redrawCurrentPage();
    void drawIntroPage();
    void pollLaunchAnimation();
    void drawLaunchFrame(uint32_t elapsedMs);
    void showPageName();
    void drawMenuPage();
    void drawMockFCPage();

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
    MockFlightComputer mockFC_;
    TelemetryStore telemetry_;

    Screen currentScreen_ = Screen::INTRO;
    OperatingMode activeMode_ = OperatingMode::RECOVERY;

    uint8_t menuSelection_ = 0; // 0 for Recovery, 1 for MockFC
    static constexpr uint8_t MODE_COUNT = 2;

    uint8_t displayOffset_ = 0;
    size_t telemetryPageIndex_ = 0;
    bool pageNameVisible_ = false;
    uint32_t pageNameStartedMs_ = 0;
    uint32_t displayedTelemetryRevision_ = 0;
    uint32_t lastRadioInitAttemptMs_ = 0;
    uint32_t launchStartedMs_ = 0;
    uint32_t lastLaunchFrameMs_ = 0;
};
