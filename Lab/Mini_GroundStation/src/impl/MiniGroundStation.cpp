#include "MiniGroundStation.h"

#include "drawTelemetry.h"
#include "introPage.h"

void MiniGroundStation::setup()
{
    Serial.begin(115200);
    delay(1000);

    buttonPad_.begin();
    display_.begin();
    redrawCurrentPage();

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
    if (currentPage_ == PageMode::TELEMETRY &&
        displayedTelemetryRevision_ != telemetry_.revision())
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
    if (event == ButtonPad::Event::PRESSED)
    {
        currentPage_ = currentPage_ == PageMode::INTRO
            ? PageMode::TELEMETRY
            : PageMode::INTRO;
        redrawCurrentPage();
        return;
    }

    if (currentPage_ != PageMode::TELEMETRY)
    {
        return;
    }

    constexpr size_t offsetCount =
        TELEMETRY_FIELD_COUNT > TELEMETRY_VISIBLE_FIELD_COUNT
            ? TELEMETRY_FIELD_COUNT - TELEMETRY_VISIBLE_FIELD_COUNT + 1
            : 1;

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

    drawTelemetryPage(display_, telemetry_.latest(), displayOffset_);
    displayedTelemetryRevision_ = telemetry_.revision();
}

void MiniGroundStation::drawIntroPage()
{
    display_.clearBuffer();
    display_.drawXBM(0, 0, 128, 64, intro_bits);
    display_.sendBuffer();
}
