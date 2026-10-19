"""Quick October range-test plots. Run: python RangeTestData/analyze_october_range.py"""

from pathlib import Path
import re

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "Range Test - October 2026"
OUTPUT = SOURCE / "analysis"
# Planned approximate ranges, from page 3 of the supplied test-plan PDF.
DISTANCES = {1: 11.0, 2: 16.24, 3: 25.5, 4: 34.81, 5: 43.0}
COLORS = {"GS": "#2171b5", "FC": "#d95f0e"}
METRICS = ["GS_T", "GS_RSSI", "GS_SNR", "FC_T", "FC_RSSI", "FC_SNR"]


def load_log(path):
    rows, errors, other, settings = [], 0, 0, set()
    for line in path.read_text(encoding="utf-8-sig").splitlines():
        line = line.strip()
        if not line:
            continue
        if line.startswith("ping_ack:"):
            settings.add(line.removeprefix("ping_ack:"))
            continue
        if line.startswith(("RX CRC error", "RX header error")):
            errors += 1
            continue
        try:
            fields = line.split(",")
            row = {"packet_id": int(fields[0]), "flag": fields[1]}
            row.update({k.strip(): float(v) for k, v in
                        (field.split(":", 1) for field in fields[2:])})
            if not all(key in row and np.isfinite(row[key]) for key in METRICS):
                raise ValueError("Missing or nonfinite metric")
            if row["flag"] not in ("CTS", "ACK"):
                raise ValueError("Unknown flag")
            rows.append(row)
        except (ValueError, IndexError):
            other += 1
    return pd.DataFrame(rows, columns=["packet_id", "flag"] + METRICS), errors, other, "; ".join(sorted(settings))


def samples(df, receiver, metric):
    # FC readings are taken before transmitting telemetry. ACK follows reception
    # of a ground command; CTS may repeat the previous reading or startup state.
    selected = df if receiver == "GS" else df.loc[df.flag.eq("ACK")]
    return selected[f"{receiver}_{metric}"]


def finish(fig, name, note):
    fig.text(0.5, 0.015, note, ha="center", fontsize=9, color="#555555")
    fig.tight_layout(rect=(0, 0.055, 1, 0.94))
    fig.savefig(OUTPUT / name, dpi=170)
    plt.close(fig)


def compare_runs(runs, name, title):
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    x = np.arange(len(runs))
    for ax, metric, unit in zip(axes, ["RSSI", "SNR"], ["dBm", "dB"]):
        for offset, receiver in [(-0.17, "GS"), (0.17, "FC")]:
            vals = [samples(run["df"], receiver, metric) for run in runs]
            means = np.array([v.mean() for v in vals])
            # Descriptive spread, not uncertainty in the mean.
            low = np.array([v.min() for v in vals])
            high = np.array([v.max() for v in vals])
            ax.errorbar(x + offset, means, yerr=[means-low, high-means],
                        fmt="o", capsize=4, label=receiver, color=COLORS[receiver])
        ax.set_xticks(x, [r["label"].replace(" off mountain", " off\nmountain")
                         .replace("Yagi ", "Yagi\n") for r in runs], fontsize=9)
        ax.set_ylabel(f"{metric} ({unit})")
        ax.grid(axis="y", alpha=0.25)
        ax.legend()
    fig.suptitle(title, fontsize=15)
    finish(fig, name, "Dots: mean; whiskers: observed min-max. GS: all telemetry; FC: ACK rows only.")


def main():
    OUTPUT.mkdir(exist_ok=True)
    plt.rcParams.update({"axes.spines.top": False, "axes.spines.right": False,
                         "font.size": 11})
    runs, summary, unassigned = [], [], []
    for path in sorted(SOURCE.glob("*.csv")):
        df, errors, other, settings = load_log(path)
        match = re.match(r"L([1-5])", path.stem, re.I)
        if not match:
            unassigned.append(f"- {path.name}: {len(df)} telemetry rows, {errors} RX errors; location unknown.")
            continue
        location = int(match[1])
        label = re.sub(r"^L[1-5]", "", path.stem, flags=re.I)
        label = {"20DegreesOfMountain": "20 deg off mountain",
                 "Yagi5DegreesOff": "Yagi 5 deg off",
                 "YagiAtMountain": "Yagi at mountain"}.get(label, label)
        run = dict(file=path.name, location=location, distance=DISTANCES[location],
                   label=label, df=df, settings=settings)
        runs.append(run)
        result = dict(file=path.name, location=f"L{location}", distance_km=DISTANCES[location],
                      radio_settings=settings, telemetry_rows=len(df), rx_error_rows=errors,
                      malformed_rows=other, fc_non_ack_rows_excluded=int(df.flag.ne("ACK").sum()))
        for receiver in COLORS:
            for metric in ["RSSI", "SNR"]:
                vals = samples(df, receiver, metric)
                key = f"{receiver}_{metric}"
                for stat in ["mean", "median", "min", "max", "count"]:
                    result[f"{key}_{stat}"] = getattr(vals, stat)()
                result[f"{key}_p10"] = vals.quantile(.1)
                result[f"{key}_p90"] = vals.quantile(.9)
        summary.append(result)
    stats = pd.DataFrame(summary)
    stats.to_csv(OUTPUT / "run_summary.csv", index=False, float_format="%.3f")

    baseline = sorted([r for r in runs if r["label"].lower() == "config2"],
                      key=lambda r: r["distance"])
    assert [r["location"] for r in baseline] == [1, 2, 3, 4, 5]
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    x = np.array([r["distance"] for r in baseline])
    for ax, metric, unit in zip(axes, ["RSSI", "SNR"], ["dBm", "dB"]):
        for receiver, color in COLORS.items():
            vals = [samples(r["df"], receiver, metric) for r in baseline]
            means = [v.mean() for v in vals]
            ax.plot(x, means, "o-", color=color, label=receiver)
            ax.fill_between(x, [v.quantile(.1) for v in vals],
                            [v.quantile(.9) for v in vals], color=color, alpha=.13)
            for distance, mean in zip(x, means):
                ax.annotate(f"{mean:.1f}", (distance, mean), xytext=(0, -18 if receiver == "GS" else 10),
                            textcoords="offset points", ha="center", color=color, fontsize=9)
        ax.set_xticks(x, [f"L{r['location']}\n{r['distance']:g}" for r in baseline])
        ax.set_xlabel("Planned range (km)")
        ax.set_ylabel(f"{metric} ({unit})")
        ax.grid(alpha=.25)
        ax.margins(y=.23)
        ax.legend()
    fig.suptitle("October range test: Config 2 (SF10, 125 kHz)", fontsize=15)
    finish(fig, "rssi_snr_vs_distance.png",
           "Mean with 10th-90th percentile shading. GS: all telemetry; FC: ACK rows only. Lines connect separate stops.")

    compare_runs([r for r in runs if r["location"] == 1], "L1_configurations.png",
                 "L1 (11 km): configuration comparison")
    compare_runs([r for r in runs if r["location"] == 5], "L5_comparison.png",
                 "L5 (43 km): configuration and pointing comparison")

    # One time-series figure per stop, with a row for each recorded run.
    for location, distance in DISTANCES.items():
        group = [r for r in runs if r["location"] == location]
        fig, axes = plt.subplots(len(group), 2, figsize=(12, max(3.6, 2.5*len(group))), squeeze=False)
        for row_axes, run in zip(axes, group):
            df = run["df"]
            for ax, metric, unit in zip(row_axes, ["RSSI", "SNR"], ["dBm", "dB"]):
                for receiver, color in COLORS.items():
                    selected = df if receiver == "GS" else df.loc[df.flag.eq("ACK")]
                    ax.plot(selected.GS_T - df.GS_T.iloc[0], selected[f"{receiver}_{metric}"],
                            ".-", markersize=3, linewidth=1, color=color, label=receiver)
                ax.set_title(run["label"], fontsize=11)
                ax.set_xlabel("Elapsed time in log (s)")
                ax.set_ylabel(f"{metric} ({unit})")
                ax.grid(alpha=.25)
                ax.legend(loc="best", fontsize=9)
        fig.suptitle(f"L{location}: {distance:g} km", fontsize=15)
        finish(fig, f"L{location}_time_series.png",
               "Raw readings; each run starts at 0 s. FC uses ACK rows to avoid startup/stale CTS readings.")

    lines = ["# October 18, 2026 range test", "",
             "Distances are approximate planned ranges from page 3 of `../Radio October Range Test Plan - Google Docs.pdf`.",
             "Files are mapped to stops only when their names start with L1-L5. Source CSVs are unchanged.", "",
             "## Config 2 across distance", "",
             "| Stop | Range (km) | GS RSSI (dBm) | FC RSSI (dBm) | GS SNR (dB) | FC SNR (dB) | GS / FC samples |",
             "|---|---:|---:|---:|---:|---:|---:|"]
    for run in baseline:
        s = stats.loc[stats.file.eq(run["file"])].iloc[0]
        lines.append(f"| L{run['location']} | {run['distance']:g} | {s.GS_RSSI_mean:.1f} | {s.FC_RSSI_mean:.1f} | {s.GS_SNR_mean:.1f} | {s.FC_SNR_mean:.1f} | {int(s.GS_RSSI_count)} / {int(s.FC_RSSI_count)} |")
    lines += ["", "## Interpretation and method", "",
              "- From 11 to 43 km, Config 2 mean GS RSSI falls from -89.0 to -102.0 dBm (13.0 dB), and FC RSSI falls from -88.7 to -98.9 dBm (10.3 dB). GS SNR falls from +8.4 to +5.5 dB, and FC SNR from +4.9 to -4.0 dB.",
              "- The trend is not monotonic: L3 and L4 recover relative to L2. The clearest deterioration is at L5, which also has much wider packet-to-packet spread.",
              "- At L5, the Yagi 5-degree-off run averages -108.9 dBm GS RSSI versus -103.1 dBm pointed at the mountain, a 5.8 dB difference. GS SNR differs by 5.0 dB. These are separate runs and show association, not an isolated controlled estimate of pointing loss.",
              "- Config 2 is the like-config comparison available at all five stops. Range alone does not explain differences between sites or antenna placement.",
              "- Mean RSSI is the arithmetic mean of logged dBm readings. Shading is packet spread (10th-90th percentile), not a confidence interval.",
              "- GS metrics use valid CTS and ACK telemetry. FC metrics use ACK rows only: the FC measures its previous reception before sending, so CTS can repeat a reading or contain startup state (often -25 dBm). See GroundStation/src/variant/RangeTestFCVariant.cpp.",
              "- RX CRC/header error rows and malformed lines are counted in run_summary.csv but excluded from signal averages. The final valid row is retained. Failed receptions are not represented in the signal curves; these plots do not establish packet delivery rate or maximum range.",
              "- Config 3 logs report SF12 / 62.5 kHz; the plan lists SF10 / 62.5 kHz. Config 1 reports SF8 / 125 kHz; Config 2 reports SF10 / 125 kHz. Logged ping settings report 433 MHz (the plan describes the 435 band).",
              "- L1Config3Pt2 is kept separate. L5 antenna-pointing runs are kept separate from the Config 2 distance trend. Antenna setup for other named configuration runs is not established by the filenames.",
              "- Per-stop time plots show each run independently; elapsed time is not inferred distance.", "",
              "## Unassigned logs", "", *unassigned, "",
              "Re-run from the repository root: `python RangeTestData/analyze_october_range.py`."]
    (OUTPUT / "analysis.md").write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(stats[["file", "telemetry_rows", "rx_error_rows", "malformed_rows", "GS_RSSI_mean", "FC_RSSI_mean", "GS_SNR_mean", "FC_SNR_mean"]].to_string(index=False))
    print(f"\nSaved plots and summary to {OUTPUT}")


if __name__ == "__main__":
    main()
