#include "MiniGroundStation.h"

#include "drawTelemetry.h"
#include "introPage.h"

void MiniGroundStation::setup()
{
    Serial.begin(115200);
    delay(1000);

    buttonPad_.begin();
    display_.begin();
    showPageName();

    lastRadioInitAttemptMs_ = millis();
    if (radio_.begin())
    {
        Serial.println("Mini ground station ready in receive-only mode");
    }
}

void MiniGroundStation::poll()
{
    pollRadio();
    pollButtons();
    if (pageNameVisible_ && millis() - pageNameStartedMs_ >= PAGE_NAME_DURATION_MS)
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
    if (event == ButtonPad::Event::MIDDLE_LONG_PRESSED)
    {
        
    }

    if (event == ButtonPad::Event::MIDDLE)
    {
        currentPage_ = currentPage_ == PageMode::INTRO
            ? PageMode::TELEMETRY
            : PageMode::INTRO;
        if (currentPage_ == PageMode::TELEMETRY)
        {
            showPageName();
        }
        else
        {
            pageNameVisible_ = false;
            redrawCurrentPage();
        }
        return;
    }

    if (currentPage_ != PageMode::TELEMETRY)
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
    if (currentPage_ == PageMode::INTRO)
    {
        drawIntroPage();
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

void MiniGroundStation::drawModeMenuPage()
{
    display_.clearBuffer();

}
