#include "MiniGroundStation.h"

#include "drawTelemetry.h"
#include "introPage.h"

void MiniGroundStation::setup()
{
    Serial.begin(115200);
    delay(1000);

    buttonPad_.begin();
    display_.begin();
    if (currentScreen_ == Screen::MODE &&
    activeMode_ == OperatingMode::RECOVERY)
    {
        showPageName();
    }
    else
    {
        redrawCurrentPage();
    }

    lastRadioInitAttemptMs_ = millis();
    if (radio_.begin())
    {
        Serial.println("Mini ground station radio ready");
    }
}

void MiniGroundStation::poll()
{
    pollButtons();
    pollRadio();
    if (currentScreen_ == Screen::MODE && activeMode_ == OperatingMode::RECOVERY &&
        pageNameVisible_ && millis() - pageNameStartedMs_ >= PAGE_NAME_DURATION_MS)
    {
        pageNameVisible_ = false;
        redrawCurrentPage();
    }
}

void MiniGroundStation::pollRadio()
{
    if (!radio_.isReady())
    {
        const uint32_t now = millis();
        if (now - lastRadioInitAttemptMs_ >= RADIO_RETRY_INTERVAL_MS)
        {
            lastRadioInitAttemptMs_ = now;
            radio_.begin();
        }
        return;
    }

    if (activeMode_ == OperatingMode::MOCKFC)
    {
        if (currentScreen_ == Screen::MODE && mockFC_.poll(radio_))
        {
            redrawCurrentPage();
        }
        return;
    }

    radio_.poll(telemetry_);
    if (currentScreen_ == Screen::MODE && activeMode_ == OperatingMode::RECOVERY && 
        !pageNameVisible_ && displayedTelemetryRevision_ != telemetry_.revision())
    {
        redrawCurrentPage();
    }
}

void MiniGroundStation::pollButtons()
{
    buttonPad_.update();

    ButtonPad::Event event;
    while (buttonPad_.getInput(event))
    {
        Serial.print("Button event: ");
        Serial.println(buttonPad_.eventToString(event));
        handleButtonEvent(event);
    }
}

void MiniGroundStation::handleButtonEvent(ButtonPad::Event event)
{
    if (event == ButtonPad::Event::MIDDLE_LONG_PRESSED) // Toggle between INTRO and MENU screens
    {
        if (currentScreen_ == Screen::INTRO)
        {
            currentScreen_ = Screen::MENU;
        }
        else
        {
            currentScreen_ = Screen::INTRO;
        }

        pageNameVisible_ = false;
        redrawCurrentPage();
        return;
    }

    if (currentScreen_ == Screen::INTRO) // Only long press can exit INTRO
    {
        return;
    }

    if (currentScreen_ == Screen::MENU)
    {
        if (event == ButtonPad::Event::UP)
        {
            menuSelection_ = (menuSelection_ + MODE_COUNT - 1) % MODE_COUNT;
        }
        else if (event == ButtonPad::Event::DOWN)
        {
            menuSelection_ = (menuSelection_ + 1) % MODE_COUNT;
        }
        else if (event == ButtonPad::Event::MIDDLE) // Select the current menu option
        {
            if (menuSelection_ == 0)
            {
                activeMode_ = OperatingMode::RECOVERY;
            }
            else if (menuSelection_ == 1)
            {
                activeMode_ = OperatingMode::MOCKFC;
            }
            currentScreen_ = Screen::MODE;
            pageNameVisible_ = false;

            if (activeMode_ == OperatingMode::RECOVERY)
            {
                telemetryPageIndex_ = 0;
                displayOffset_ = 0;
                showPageName();
            }
            else
            {
                mockFC_.reset();
                redrawCurrentPage();
            }
            return;
        }
        redrawCurrentPage();
        return;
    }

    if (event == ButtonPad::Event::MIDDLE) // Exit to MENU screen from MODE
    {
        currentScreen_ = Screen::MENU;
        pageNameVisible_ = false;
        drawMenuPage();
        return;
    }

    if (currentScreen_ != Screen::MODE || activeMode_ != OperatingMode::RECOVERY)
    {
        return;
    }

    if (event == ButtonPad::Event::LEFT || event == ButtonPad::Event::RIGHT)
    {
        telemetryPageIndex_ = event == ButtonPad::Event::RIGHT
            ? (telemetryPageIndex_ + 1) % TELEMETRY_PAGE_COUNT
            : (telemetryPageIndex_ + TELEMETRY_PAGE_COUNT - 1) % TELEMETRY_PAGE_COUNT;
        displayOffset_ = 0;
        showPageName();
        return;
    }

    const auto& page = TELEMETRY_PAGES[telemetryPageIndex_];
    const size_t offsetCount = page.fieldCount > TELEMETRY_VISIBLE_FIELD_COUNT
        ? page.fieldCount - TELEMETRY_VISIBLE_FIELD_COUNT + 1 : 1;

    if (event == ButtonPad::Event::UP)
    {
        displayOffset_ = static_cast<uint8_t>(
            (displayOffset_ + offsetCount - 1) % offsetCount);
        redrawCurrentPage();
    }
    else if (event == ButtonPad::Event::DOWN)
    {
        displayOffset_ = static_cast<uint8_t>(
            (displayOffset_ + 1) % offsetCount);
        redrawCurrentPage();
    }
}

void MiniGroundStation::redrawCurrentPage()
{
    if (currentScreen_ == Screen::INTRO)
    {
        drawIntroPage();
        return;
    }

    if (currentScreen_ == Screen::MENU)
    {
        drawMenuPage();
        return;
    }

    if (activeMode_ == OperatingMode::MOCKFC)
    {
        drawMockFCPage();
        return;
    }

    if (pageNameVisible_)
    {
        drawTelemetryPageName(display_, TELEMETRY_PAGES[telemetryPageIndex_].name);
        return;
    }

    drawTelemetryPage(display_, telemetry_.latest(),
                      TELEMETRY_PAGES[telemetryPageIndex_], displayOffset_);
    displayedTelemetryRevision_ = telemetry_.revision();
}

void MiniGroundStation::drawMockFCPage()
{
    display_.clearBuffer();
    display_.setFont(u8g2_font_6x10_tr);
    display_.drawStr(0, 10, "MockFC Mode");
    display_.setCursor(0, 22);
    display_.print("Packet: ");
    display_.print(mockFC_.lastSequence());
    display_.setCursor(0, 34);
    display_.print("Flags: ");
    display_.print(mockFC_.lastFlags());
    display_.print(" ACK ID: ");
    display_.print(mockFC_.ackId());
    display_.drawStr(0, 46, radio_.isReady() ? mockFC_.status() : "Radio unavailable");
    display_.drawStr(0, 62, "Press: exit to menu");
    display_.sendBuffer();
}

void MiniGroundStation::drawIntroPage()
{
    display_.clearBuffer();
    display_.drawXBM(0, 0, 128, 64, intro_bits);
    display_.sendBuffer();
}

void MiniGroundStation::showPageName()
{
    pageNameVisible_ = true;
    pageNameStartedMs_ = millis();
    redrawCurrentPage();
}

void MiniGroundStation::drawMenuPage()
{
    static const char* labels[] = { "Recovery Mode", "MockFC Mode" };

    display_.clearBuffer();
    display_.setFont(u8g2_font_ncenB08_tr);
    display_.drawStr(0, 10, "Select Mode:");

    for (uint8_t i = 0; i < MODE_COUNT; ++i)
    {
        const int y = 28 + i * 12;

        if (i == menuSelection_)
        {
            display_.drawStr(0, y, ">");
        }

        display_.drawStr(12, y, labels[i]);
    }

    display_.sendBuffer();
}
