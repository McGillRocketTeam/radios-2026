import numpy as np
import pandas as pd
from matplotlib import pyplot as plt
import io
from enum import Enum

# Path of the .txt file with the data
FILE_NAME = "./data/No_BODGE.txt"

df = pd.DataFrame(columns=["packet_id", "flag", "GS_T", "GS_RSSI", "GS_SNR", "FC_T", "FC_RSSI", "FC_SNR"])
csv_data = []

with open(FILE_NAME) as file:
    txt = file.readlines()
    csv_data = txt[3:-2]   # csv data excludes first 3 lines and last 2

raw_df = pd.read_csv(io.StringIO("".join(csv_data)), sep=',|:', engine='python', header=None)

# rows where the token before the value isn't actually GS_SNR
bad = raw_df[6].ne("GS_SNR") | raw_df[7].isna()
print("bad rows:", bad.sum(), "out of", len(raw_df))
print(raw_df.loc[bad].head(20))


# Populate dataframe with parsed values
df["packet_id"] = raw_df[0]
df["flag"]      = raw_df[1]
df["GS_T"]      = raw_df[3]
df["GS_RSSI"]   = raw_df[5]
df["GS_SNR"]    = raw_df[7]
df["FC_T"]      = raw_df[9]
df["FC_RSSI"]   = raw_df[11]
df["FC_SNR"]    = raw_df[13]

X = np.arange(len(csv_data))

class System(Enum):
    Ground_Station = "GS"
    Flight_Computer = "FC"

class Subplot:
    rssi_range = (-100, 10)
    snr_range = (-20, 20)
    color_rssi = 'tab:red'
    color_snr = "tab:blue"

    def __init__(self, system: System):
        self.system = system
        fig, (ax_rssi, ax_snr) = plt.subplots(1, 2)

        self.fig = fig 
        self.ax_rssi = ax_rssi 
        self.ax_snr = ax_snr 

        self.fig.suptitle(system.name, fontweight="bold", fontsize=14)
        self.title_row = self.fig.add_subplot(211, frameon=False)
        self.title_row.set_title(FILE_NAME)
        self.title_row.axis("off")
    
    def plot_rssi(self):
        self.ax_rssi.set_xlabel("Time")
        self.ax_rssi.set_ylabel("RSSI")
        self.ax_rssi.set_ylim(*self.rssi_range)
        self.ax_rssi.plot(X, df[f"{self.system.value}_RSSI"], color=self.color_rssi)
    
    def plot_snr(self):
        self.ax_snr.set_xlabel("Time")
        self.ax_snr.set_ylabel("SNR")
        self.ax_snr.set_ylim(*self.snr_range)
        self.ax_snr.yaxis.set_label_position("right")
        self.ax_snr.yaxis.tick_right()
        self.ax_snr.plot(X, df[f"{self.system.value}_SNR"], color=self.color_snr)

    def show(self):
        self.fig.tight_layout(w_pad=1.0)
        self.fig.show()
    

gs_subplot = Subplot(System.Ground_Station)
gs_subplot.plot_rssi()
gs_subplot.plot_snr()

fc_subplot = Subplot(System.Flight_Computer)
fc_subplot.plot_rssi()
fc_subplot.plot_snr()

plt.show()