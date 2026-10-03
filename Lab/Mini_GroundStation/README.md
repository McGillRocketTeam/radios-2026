# Mini ground station

Build/upload the `teensy40` environment here. On the transmitting ground station,
build/upload `GroundStation/platformio.ini`'s `FC_Mock` environment. Both devices
use the shared radio settings in `GroundStation/include/configs/LoraParamConfig.h`
and must use the same hardware frequency band.

Press the button pad to open telemetry; use up/down to scroll. Mini GS only
receives, so FC_Mock's CTS response timeouts are expected and it continues sending.

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

The current mock generator uses tiny raw GPS values (0.7, 0.8, 0.9), so the display
rounds these to `0.000000 deg`, `0.000000 deg`, and `0.00 m`. GPS age is `1.0 s`,
barometer altitude is `0.1 ft`, FC RSSI is `-1.0 dBm`, and FC SNR is `0.00 dB`.
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
