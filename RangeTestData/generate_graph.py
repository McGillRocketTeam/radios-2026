import numpy as np
import pandas as pd
from matplotlib import pyplot as plt
import io
from enum import Enum

# Path of the .txt file with the data
FILE_NAME = "./data/No_BODGE.txt"

class Data:
    def __init__(self, file_name):
        self.df = pd.DataFrame(columns=["packet_id", "flag", "GS_T", "GS_RSSI", "GS_SNR", "FC_T", "FC_RSSI", "FC_SNR"])

        csv_data = []
        with open(file_name) as file:
            txt = file.readlines()
            csv_data = txt[3:-2]   # csv data excludes first 3 lines and last 2

        self.raw_df = pd.read_csv(io.StringIO("".join(csv_data)), sep=',|:', engine='python', header=None)

        # Populate dataframe with parsed values
        self.df["packet_id"] = self.raw_df[0]
        self.df["flag"]      = self.raw_df[1]
        self.df["GS_T"]      = self.raw_df[3]
        self.df["GS_RSSI"]   = self.raw_df[5]
        self.df["GS_SNR"]    = self.raw_df[7]
        self.df["FC_T"]      = self.raw_df[9]
        self.df["FC_RSSI"]   = self.raw_df[11]
        self.df["FC_SNR"]    = self.raw_df[13]
    
    def count_bad_rows(self):
        # rows where the token before the value isn't actually GS_SNR
        bad = self.raw_df[6].ne("GS_SNR") | self.raw_df[7].isna()
        print("bad rows:", bad.sum(), "out of", len(self.raw_df))
        print(self.raw_df.loc[bad].head(20))

    def packet_drop(self):
        # packet drop means that two CTS were received consecutively, without an ACK in between
        
        # find all rows with CTS flag
        is_cts = (self.df["flag"] == "CTS")

        # find all the 'next' rows that are not ACK
        next_is_not_ack = (self.df["flag"].shift(-1) != "ACK")

        packets_dropped = self.df[is_cts & next_is_not_ack]
        return packets_dropped

    def count_packet_drop(self):
        packets_dropped = self.packet_drop()

        print(f"{len(packets_dropped)} packets dropped")
        print(packets_dropped.head(20))
    
    def average_cts_ack_time(self):
        # Ignore entries where the packet dropped
        packets_dropped = self.packet_drop()
        successful_tx = self.df.drop(packets_dropped.index)

        # Compute the difference between rows in GS_T
        successful_tx["tx_time"] = successful_tx["GS_T"].diff()

        # Keep only the rows with flag=ACK. This gives us the difference ACK-CTS
        # for each packet that was successfully sent
        handshake_duration = successful_tx[successful_tx["flag"] == "ACK"]["tx_time"]

        # DataFrame handshake_duration can be plotted if necessary
        print("CTS-ACK handshake statistics:")
        print(handshake_duration.describe())

class System(Enum):
    Ground_Station = "GS"
    Flight_Computer = "FC"

class Subplot:
    rssi_range = (-100, 10)
    snr_range = (-20, 20)
    color_rssi = 'tab:red'
    color_snr = "tab:blue"

    def __init__(self, system: System, df: pd.DataFrame):
        self.df = df
        self.X = np.arange(len(df))

        self.system = system
        fig, (ax_rssi, ax_snr) = plt.subplots(1, 2)

        self.fig = fig 
        self.ax_rssi = ax_rssi 
        self.ax_snr = ax_snr 

        self.fig.suptitle(system.name.replace("_", " "), fontweight="bold", fontsize=14)
        self.title_row = self.fig.add_subplot(211, frameon=False)
        self.title_row.set_title(FILE_NAME)
        self.title_row.axis("off")
    
    def plot_rssi(self):
        self.ax_rssi.set_xlabel("Time")
        self.ax_rssi.set_ylabel("RSSI")
        self.ax_rssi.set_ylim(*self.rssi_range)
        self.ax_rssi.plot(self.X, self.df[f"{self.system.value}_RSSI"], color=self.color_rssi)
    
    def plot_snr(self):
        self.ax_snr.set_xlabel("Time")
        self.ax_snr.set_ylabel("SNR")
        self.ax_snr.set_ylim(*self.snr_range)
        self.ax_snr.yaxis.set_label_position("right")
        self.ax_snr.yaxis.tick_right()
        self.ax_snr.plot(self.X, self.df[f"{self.system.value}_SNR"], color=self.color_snr)

    def show(self):
        self.fig.tight_layout(w_pad=1.0)
        self.fig.show()
    
data = Data(FILE_NAME)

data.count_bad_rows()
print("=====================")
data.count_packet_drop()
print("=====================")
data.average_cts_ack_time()

gs_subplot = Subplot(System.Ground_Station, data.df)
gs_subplot.plot_rssi()
gs_subplot.plot_snr()

fc_subplot = Subplot(System.Flight_Computer, data.df)
fc_subplot.plot_rssi()
fc_subplot.plot_snr()

plt.show()