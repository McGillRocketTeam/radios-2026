#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "peripheralPins.h"
#include "telemetryInfo.h"
#include "drawTelemetry.h"


U8G2_SSD1309_128X64_NONAME0_F_2ND_HW_I2C display(
    U8G2_R0,
    DISPLAY_RST
);

TelemetryInfo info =
{
    45.5017f,     // gps_latitude_deg
    -73.5673f,    // gps_longitude_deg
    32.4f,        // gps_altitude_m
    1.2f,         // gps_time_last_update_s
    118.7f,       // baro_altitude_ft
    9932,         // packet_sequence_number
    87,           // rssi_dbm
    9             // snr_db
};

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Wire.begin();
    Wire.setClock(100000);

    display.setI2CAddress(0x3C << 1);
    display.begin();

    drawTelemetryPage(display, info, 0);
}

void loop()
{
}