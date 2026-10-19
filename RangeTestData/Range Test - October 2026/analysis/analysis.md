# October 18, 2026 range test

Distances are approximate planned ranges from page 3 of `../Radio October Range Test Plan - Google Docs.pdf`.
Files are mapped to stops only when their names start with L1-L5. Source CSVs are unchanged.

## Config 2 across distance

| Stop | Range (km) | GS RSSI (dBm) | FC RSSI (dBm) | GS SNR (dB) | FC SNR (dB) | GS / FC samples |
|---|---:|---:|---:|---:|---:|---:|
| L1 | 11 | -89.0 | -88.7 | 8.4 | 4.9 | 97 / 48 |
| L2 | 16.24 | -93.3 | -92.4 | 7.9 | 4.0 | 89 / 44 |
| L3 | 25.5 | -91.4 | -90.5 | 8.2 | 6.6 | 73 / 36 |
| L4 | 34.81 | -91.0 | -88.2 | 8.1 | 6.4 | 63 / 29 |
| L5 | 43 | -102.0 | -98.9 | 5.5 | -4.0 | 120 / 58 |

## Interpretation and method

- From 11 to 43 km, Config 2 mean GS RSSI falls from -89.0 to -102.0 dBm (13.0 dB), and FC RSSI falls from -88.7 to -98.9 dBm (10.3 dB). GS SNR falls from +8.4 to +5.5 dB, and FC SNR from +4.9 to -4.0 dB.
- The trend is not monotonic: L3 and L4 recover relative to L2. The clearest deterioration is at L5, which also has much wider packet-to-packet spread.
- At L5, the Yagi 5-degree-off run averages -108.9 dBm GS RSSI versus -103.1 dBm pointed at the mountain, a 5.8 dB difference. GS SNR differs by 5.0 dB. These are separate runs and show association, not an isolated controlled estimate of pointing loss.
- Config 2 is the like-config comparison available at all five stops. Range alone does not explain differences between sites or antenna placement.
- Mean RSSI is the arithmetic mean of logged dBm readings. Shading is packet spread (10th-90th percentile), not a confidence interval.
- GS metrics use valid CTS and ACK telemetry. FC metrics use ACK rows only: the FC measures its previous reception before sending, so CTS can repeat a reading or contain startup state (often -25 dBm). See GroundStation/src/variant/RangeTestFCVariant.cpp.
- RX CRC/header error rows and malformed lines are counted in run_summary.csv but excluded from signal averages. The final valid row is retained. Failed receptions are not represented in the signal curves; these plots do not establish packet delivery rate or maximum range.
- Config 3 logs report SF12 / 62.5 kHz; the plan lists SF10 / 62.5 kHz. Config 1 reports SF8 / 125 kHz; Config 2 reports SF10 / 125 kHz. Logged ping settings report 433 MHz (the plan describes the 435 band).
- L1Config3Pt2 is kept separate. L5 antenna-pointing runs are kept separate from the Config 2 distance trend. Antenna setup for other named configuration runs is not established by the filenames.
- Per-stop time plots show each run independently; elapsed time is not inferred distance.

## Unassigned logs

- test_18_09_14.csv: 23 telemetry rows, 0 RX errors; location unknown.
- test_18_12_32.csv: 0 telemetry rows, 0 RX errors; location unknown.
- test_18_12_33.csv: 69 telemetry rows, 0 RX errors; location unknown.
- test_18_12_43.csv: 20 telemetry rows, 0 RX errors; location unknown.
- test_18_12_45.csv: 0 telemetry rows, 0 RX errors; location unknown.
- test_18_14_33.csv: 0 telemetry rows, 0 RX errors; location unknown.

Re-run from the repository root: `python RangeTestData/analyze_october_range.py`.
