#include <Arduino.h>
#include <U8g2lib.h>

#include "Joystick.h"
#include "peripheralPins.h"
#include "telemetryInfo.h"
#include "drawTelemetry.h"
#include "introPage.h"

U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI display(
    U8G2_R0,
    DISPLAY_CS_PIN,
    DISPLAY_DC_PIN,
    DISPLAY_RST_PIN
);

Joystick joystick(STICK_VRX_PIN, STICK_VRY_PIN, STICK_SW_PIN);

TelemetryInfo info =
{
    45.5017f,   // gps_latitude_deg
    -73.5673f,  // gps_longitude_deg
    32.4f,      // gps_altitude_m
    1.2f,       // gps_time_last_update_s
    118.7f,     // baro_altitude_ft
    9932,       // packet_sequence_number
    87,         // rssi_dbm
    9           // snr_db
};

static uint8_t displayOffset = 0;

enum class PageMode
{
    INTRO,
    TELEMETRY
};

static PageMode currentPage = PageMode::TELEMETRY;

void drawIntroPage(U8G2& display)
{
    display.clearBuffer();
    display.drawXBM(0, 0, 128, 64, intro_bits);
    display.sendBuffer();
}

void redrawCurrentPage()
{
    if (currentPage == PageMode::INTRO)
    {
        drawIntroPage(display);
    }
    else
    {
        drawTelemetryPage(display, info, displayOffset);
    }
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    joystick.begin();

    display.begin();

    redrawCurrentPage();
}

void loop()
{
    joystick.update();

    Joystick::Event event;
    if (joystick.getInput(event))
    {
        Serial.print("New event: ");
        Serial.println(joystick.eventToString(event));

        if (event == Joystick::Event::PRESSED)
        {
            currentPage = (currentPage == PageMode::INTRO)
                ? PageMode::TELEMETRY
                : PageMode::INTRO;

            redrawCurrentPage();
            return;
        }

        if (currentPage == PageMode::TELEMETRY)
        {
            if (event == Joystick::Event::UP)
            {
                displayOffset =
                    (displayOffset + TELEMETRY_FIELD_COUNT - 1) % (TELEMETRY_FIELD_COUNT - 4);

                drawTelemetryPage(display, info, displayOffset);
            }
            else if (event == Joystick::Event::DOWN)
            {
                displayOffset =
                    (displayOffset + 1) % (TELEMETRY_FIELD_COUNT - 4);

                drawTelemetryPage(display, info, displayOffset);
            }
        }
    }
}