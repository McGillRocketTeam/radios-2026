# Mini ground station

Build/upload the `teensy40` environment here. On the transmitting ground station,
build/upload `GroundStation/platformio.ini`'s `FC_Mock` environment. Both devices
use the shared radio settings in `GroundStation/include/configs/LoraParamConfig.h`
and must use the same hardware frequency band.

Left/right cycle between Signal quality (FC/GS RSSI, FC/GS SNR, packet number)
and Flight computer (GPS latitude, longitude, altitude, age, barometer altitude).
Each page name appears for 800 ms on entry, then gives way to its parameters.
Radio reception continues during the title. Up/down scroll within the current
page if more than five parameters are configured. Press toggles the intro image.
In Recovery Mode, Mini GS only receives, so FC_Mock's CTS response timeouts are
expected and it continues sending.

## MockFC mode

Long-press the middle button to open the mode menu from the intro screen, select
`MockFC Mode` with up/down, and press the middle button to start. Mini GS now acts
as the transmitting flight computer, using the same generated telemetry and radio
settings as GroundStation's `FC_Mock`. A receiving ground station must use the
same frequency band. The screen shows the last successfully transmitted packet,
flags (CTS = 1, ACK = 2, NAK = 8), command ACK ID, and current status.

The simulator sends a packet without CTS, then CTS packets. After each CTS it
listens for a command for one second; if none arrives, it retries. Normal and
extended command packets receive an ACK with their command ID; `nop` receives a
NAK. Commands are simulated acknowledgements and are only printed to serial.
Response waits are polled so the menu remains usable; radio transmission itself
is blocking, as in the original FC_Mock. Press the middle button to return to the
menu and stop mock transmissions. Selecting Recovery Mode resumes reception.
Re-entering MockFC Mode resets the sequence to 1.

The decoder uses GroundStation's generated packet structs and size catalog.
Displayed units follow the sibling `ground-station` repo's
`apps/backend/src/main/yamcs/mdb/rocket.xml` calibrators:

| Display | Wire value conversion |
| --- | --- |
| GPS latitude/longitude (degrees) | divide by 10,000,000 |
| GPS altitude (metres) | divide by 1000 |
| GPS age (seconds) | unchanged |
| Barometer altitude from pad (feet) | unchanged |
| FC RSSI (dBm) | divide by -2 |
| FC SNR (dB) | divide by 4 |

The shared mock generator uses tiny raw GPS values (0.7, 0.8, 0.9). Both the
GroundStation FC_Mock and Mini GS MockFC modes overwrite latitude/longitude with
the original simulator's moving values near `100 deg` / `-100 deg`, and GPS time
with uptime in seconds. These are synthetic coordinates (the latitude is outside
the geographic range). GPS altitude rounds to `0.00 m`; barometer altitude is
`0.1 ft`, FC RSSI is `-1.0 dBm`, and FC SNR is `0.00 dB`.
The packet sequence advances on reception. FC RSSI/SNR are values reported by the
FC, not the mini GS receiver's local signal measurements.

## Host regression test

From the repository root, with g++ available (PowerShell):

```powershell
g++ -std=c++17 -Wall -Wextra -I Lab/Mini_GroundStation/test/telemetry_store -I Lab/Mini_GroundStation/include -I GroundStation/lib/telemetry Lab/Mini_GroundStation/test/telemetry_store/test_main.cpp Lab/Mini_GroundStation/src/impl/TelemetryStore.cpp Lab/Mini_GroundStation/src/impl/TelemetryPackets.cpp GroundStation/src/telemetry/frame_builder.cpp GroundStation/src/telemetry/telemetry_generator.cpp -o "$env:TEMP/mini-gs-telemetry-test.exe"
& "$env:TEMP/mini-gs-telemetry-test.exe"
```

This tests the real FC_Mock generator, calibrated units, partial updates, and
rejection of malformed frames without changing the last telemetry snapshot.
