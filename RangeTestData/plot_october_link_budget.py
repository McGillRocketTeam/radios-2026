"""Predicted (free-space link budget) vs measured RSSI over distance, October range test, Config 2.

Run: python RangeTestData/plot_october_link_budget.py [--yagi-gain-dbi 9.1 ...]
CSV loading/filtering is shared with analyze_october_range.py.
"""

import argparse
import re

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.ticker import NullFormatter, NullLocator
import numpy as np
import pandas as pd

from analyze_october_range import COLORS, DISTANCES, OUTPUT, SOURCE, finish, load_log, samples


# Link budget. Override any of these from the command line.
FREQ_MHZ = 435.0        # Plan band; logs report 433.00 MHz (0.04 dB FSPL difference).
TX_POWER_DBM = 30.0     # Dorji module rating from the plan; firmware sets the SX126x to 22 dBm.
WHIP_GAIN_DBI = 2.15    # Placeholder: quarter-wave/dipole-ish whip.
YAGI_GAIN_DBI = 11.0    # Provided by the team; no Yagi model/gain recorded in the repo.
CABLE_LOSS_DB = 2.0     # Placeholder: total for both ends.
STOPS_KM = {"L1": 11.0, "L2": 16.24, "L3": 25.5, "L4": 34.81, "L5": 43.0}
PLACEHOLDERS = ["WHIP_GAIN_DBI", "CABLE_LOSS_DB"]

# Config 2 as defined in the plan table and LoraParamConfig.h; checked against each log's ping_ack line.
CONFIG2_SF, CONFIG2_BW_KHZ = 10, 125.0


def fspl_db(d_km, f_mhz):
    return 20*np.log10(d_km) + 20*np.log10(f_mhz) + 32.44


def predicted_rx_dbm(d_km, p):
    return (p.tx_power_dbm + p.whip_gain_dbi + p.yagi_gain_dbi - p.cable_loss_db
            - fspl_db(d_km, p.freq_mhz))


def signal_dbm(rssi, snr):
    # RSSI measures signal + noise; remove the noise share implied by SNR.
    return rssi - 10*np.log10(1 + 10**(-snr/10))


def receiver_samples(df, receiver):
    rssi = samples(df, receiver, "RSSI").to_numpy()
    snr = samples(df, receiver, "SNR").to_numpy()
    return rssi, snr, signal_dbm(rssi, snr)


def load_baseline():
    runs = []
    for path in sorted(SOURCE.glob("*.csv")):
        match = re.match(r"L([1-5])(.*)", path.stem, re.I)
        if not match or match[2].lower() != "config2":
            continue
        df, _, _, settings = load_log(path)
        freq, bw, sf, cr, power = (float(v) for v in settings.split(","))
        if (sf, bw) != (CONFIG2_SF, CONFIG2_BW_KHZ):
            raise ValueError(f"{path.name}: logged SF{sf:g}/{bw:g} kHz is not Config 2")
        runs.append(dict(file=path.name, stop=f"L{match[1]}", df=df, freq=freq, power=power))
    runs.sort(key=lambda r: STOPS_KM[r["stop"]])
    assert [r["stop"] for r in runs] == list(STOPS_KM)
    return runs


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--freq-mhz", type=float, default=FREQ_MHZ)
    parser.add_argument("--tx-power-dbm", type=float, default=TX_POWER_DBM)
    parser.add_argument("--whip-gain-dbi", type=float, default=WHIP_GAIN_DBI)
    parser.add_argument("--yagi-gain-dbi", type=float, default=YAGI_GAIN_DBI)
    parser.add_argument("--cable-loss-db", type=float, default=CABLE_LOSS_DB)
    return parser.parse_args()


def main():
    p = parse_args()
    plt.rcParams.update({"axes.spines.top": False, "axes.spines.right": False,
                         "font.size": 11})
    if STOPS_KM != {f"L{k}": v for k, v in DISTANCES.items()}:
        print("Note: STOPS_KM differs from the distances in analyze_october_range.py")

    runs = load_baseline()
    x = np.array([STOPS_KM[r["stop"]] for r in runs])
    pred = predicted_rx_dbm(x, p)
    rows = []
    for run, d, pr in zip(runs, x, pred):
        for receiver in COLORS:
            rssi, snr, sig = receiver_samples(run["df"], receiver)
            # Medians and percentiles are order statistics, so dB and mW give the same result.
            rows.append(dict(stop=run["stop"], side=receiver, distance_km=d,
                             fspl_db=fspl_db(d, p.freq_mhz), pred_rx_dbm=pr,
                             rssi_med_dbm=np.median(rssi), rssi_p10=np.quantile(rssi, .1),
                             rssi_p90=np.quantile(rssi, .9), snr_med_db=np.median(snr),
                             sig_med_dbm=np.median(sig), excess_loss_db=pr - np.median(sig),
                             n=len(rssi)))
    table = pd.DataFrame(rows)

    budget = (f"{p.tx_power_dbm:g} dBm TX + {p.whip_gain_dbi:g} dBi whip + {p.yagi_gain_dbi:g} dBi Yagi"
              f" - {p.cable_loss_db:g} dB cable, {p.freq_mhz:g} MHz")
    held = [name for name in PLACEHOLDERS
            if getattr(p, name.lower()) == globals()[name]]
    note = budget + ("\nPlaceholders: " + ", ".join(held) if held else "")
    title = f"October range test: Config 2 (SF{CONFIG2_SF}, {CONFIG2_BW_KHZ:g} kHz)"
    method = "Medians. Signal = RSSI - 10log10(1 + 10^(-SNR/10)) per packet. GS: all telemetry; FC: ACK rows only."

    def setup_axes(ax):
        ax.set_xscale("log")
        ax.xaxis.set_minor_locator(NullLocator())
        ax.xaxis.set_minor_formatter(NullFormatter())
        ax.set_xticks(x, [f"{r['stop']}\n{d:g}" for r, d in zip(runs, x)])
        # Small padding past L1/L5 so the end markers and labels are not clipped.
        ax.set_xlim(x[0] / 1.03, x[-1] * 1.03)
        ax.set_xlabel("Planned range (km, log scale)")
        ax.grid(alpha=.25)
        ax.text(0.99, 0.98, note, transform=ax.transAxes, ha="right", va="top", fontsize=8.5,
                color="#555555", bbox=dict(boxstyle="round", facecolor="white", edgecolor="#cccccc"))

    fig, ax = plt.subplots(figsize=(10, 6))
    d_line = np.geomspace(x[0], x[-1], 200)
    ax.plot(d_line, predicted_rx_dbm(d_line, p), color="#444444", linewidth=1.6,
            label="Predicted $P_{rx}$ (free space)")
    for receiver, color in COLORS.items():
        t = table.loc[table.side.eq(receiver)]
        ax.plot(t.distance_km, t.rssi_med_dbm, "o-", color=color, label=f"{receiver} median RSSI")
        ax.fill_between(t.distance_km, t.rssi_p10, t.rssi_p90, color=color, alpha=.13)
        ax.plot(t.distance_km, t.sig_med_dbm, "o--", color=color, markerfacecolor="white",
                label=f"{receiver} est. signal power")
    ax.set_ylabel("Received power (dBm)")
    setup_axes(ax)
    ax.legend(fontsize=9, loc="lower left")
    fig.suptitle(f"{title}: predicted vs measured", fontsize=15)
    finish(fig, "predicted_vs_measured_rssi_vs_distance.png",
           method.replace("Medians.", "Medians; shading: RSSI 10th-90th percentile."))

    fig, ax = plt.subplots(figsize=(10, 5))
    for receiver, color in COLORS.items():
        t = table.loc[table.side.eq(receiver)]
        ax.plot(t.distance_km, t.excess_loss_db, "o-", color=color, label=receiver)
        other = table.loc[table.side.ne(receiver)].excess_loss_db.to_numpy()
        for distance, loss, other_loss in zip(t.distance_km, t.excess_loss_db, other):
            # Label above the higher of the two sides and below the lower one.
            above = loss > other_loss or (loss == other_loss and receiver == "GS")
            ax.annotate(f"{loss:.1f}", (distance, loss), xytext=(0, 10 if above else -18),
                        textcoords="offset points", ha="center", color=color, fontsize=9)
    ax.set_ylabel("Excess loss (dB)\npredicted - est. signal")
    setup_axes(ax)
    ax.margins(y=.35)
    ax.legend(loc="lower left")
    fig.suptitle(f"{title}: excess loss over free space", fontsize=15)
    finish(fig, "excess_loss_vs_distance.png", method)

    columns = {"stop": "Stop", "side": "Side", "distance_km": "Dist (km)", "fspl_db": "FSPL (dB)",
               "pred_rx_dbm": "Pred Prx (dBm)", "rssi_med_dbm": "Med RSSI (dBm)", "snr_med_db": "Med SNR (dB)",
               "sig_med_dbm": "Est signal (dBm)", "excess_loss_db": "Excess loss (dB)", "n": "N"}
    print(table[list(columns)].rename(columns=columns).to_string(index=False, float_format="%.1f"))
    print(f"\nLogged settings: {sorted({(r['freq'], r['power']) for r in runs})} (MHz, SX126x dBm)")
    print(f"Saved predicted_vs_measured_rssi_vs_distance.png and excess_loss_vs_distance.png to {OUTPUT}")


if __name__ == "__main__":
    main()
