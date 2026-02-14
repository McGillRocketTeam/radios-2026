import numpy as np
import pandas as pd
from matplotlib import pyplot as plt
import io

# Path of the .txt file with the data
FILE_NAME = "./data/NO_BODGE.txt"

df = pd.DataFrame(columns=["packet_id", "flag", "GS_T", "GS_RSSI", "GS_SNR", "FC_T", "FC_RSSI", "FC_SNR"])
csv_data = []

with open(FILE_NAME) as file:
    txt = file.readlines()
    csv_data = txt[3:-2]   # csv data excludes first 3 lines and last 2

raw_df = pd.read_csv(io.StringIO("".join(csv_data)), sep=',|:', engine='python', header=None)


# Populate dataframe with parsed values
df["packet_id"] = raw_df[0]
df["flag"]      = raw_df[1]
df["GS_T"]      = raw_df[3]
df["GS_RSSI"]   = raw_df[5]
df["GS_SNR"]    = raw_df[7]
df["FC_T"]      = raw_df[9]
df["FC_RSSI"]   = raw_df[11]
df["FC_SNR"]    = raw_df[13]


# Plot RSSI and SNR on same plot
fig, (ax_gs, ax_fc) = plt.subplots(1, 2)

X = np.arange(len(csv_data))
color_rssi = 'tab:red'
color_snr = "tab:blue"

# Ground station plot
ax_gs.set_xlabel("Time")
ax_gs.set_ylabel("Ground Station RSSI")
ax_gs.set_ylim(-100, -30)
line1, = ax_gs.plot(X, df["GS_RSSI"], color=color_rssi, label="GS_RSSI")

ax_gs_2 = ax_gs.twinx()
ax_gs_2.set_ylabel("Ground Station SNR")
ax_gs_2.set_ylim(-20, 20)
line2, = ax_gs_2.plot(X, df["GS_SNR"], color=color_snr, label="GS_SNR")

lines = [line1, line2]
labels = [l.get_label() for l in lines]
ax_gs.legend(lines, labels, loc='lower right')
ax_gs.set_title("Ground Station")

# FC plot
ax_fc.set_xlabel("Time")
ax_fc.set_ylabel("FC RSSI")
ax_fc.set_ylim(-100, -30)
line3, = ax_fc.plot(X, df["FC_RSSI"], color=color_rssi, label="FC_RSSI")

ax_fc_2 = ax_fc.twinx()
color_snr = "tab:blue"
ax_fc_2.set_ylabel("FC SNR")
ax_fc_2.set_ylim(-20, 20)
line4, = ax_fc_2.plot(X, df["FC_SNR"], color=color_snr, label="FC_SNR")

lines = [line3, line4]
labels = [l.get_label() for l in lines]
ax_fc.legend(lines, labels, loc='lower right')
ax_fc.set_title("Flight Computer")

fig.suptitle(FILE_NAME)

plt.tight_layout(w_pad=1.0)
plt.show()