import pandas as pd

FILE_NAME = "./data/RANGE_TEST_Apr_2026/omni_433.csv"


class Data:
    def __init__(self, file_name: str):
        self.file_name = file_name
        self.raw_df = self._load_raw_data()
        self.df = self._build_clean_dataframe()

    def _load_raw_data(self) -> pd.DataFrame:
        with open(self.file_name, "r", encoding="utf-8") as file:
            txt = file.readlines()
        # Note that we ignore the last row since disconnecting power 
        # at the end of the test will result in a rx error
        csv_data = txt[:-1]

        parsed_rows = []
        max_len = 0

        for line in csv_data:
            line = line.strip()

            if not line:
                continue

            # Split on commas first, then split each piece on colon
            parts = []
            for chunk in line.split(","):
                subparts = chunk.split(":")
                for sub in subparts:
                    parts.append(sub.strip())

            parsed_rows.append(parts)
            max_len = max(max_len, len(parts))

        # Pad rows so DataFrame can be built safely
        padded_rows = []
        for row in parsed_rows:
            padded_rows.append(row + [pd.NA] * (max_len - len(row)))

        return pd.DataFrame(padded_rows)

    def _build_clean_dataframe(self) -> pd.DataFrame:
        raw = self.raw_df.copy()

        first_col = raw[0].astype(str).str.strip()

        is_ping_ack = first_col.str.contains("ping_ack", case=False, na=False)
        is_crc_error = first_col.str.contains(r"RX header error|RX CRC error", case=False, na=False)

        df = pd.DataFrame(index=raw.index)

        df["is_ping_ack"] = is_ping_ack
        df["is_crc_error"] = is_crc_error
        df["is_special_row"] = df["is_ping_ack"] | df["is_crc_error"]

        columns_map = {
            "packet_id": 0,
            "flag": 1,
            "GS_T_label": 2,
            "GS_T": 3,
            "GS_RSSI_label": 4,
            "GS_RSSI": 5,
            "GS_SNR_label": 6,
            "GS_SNR": 7,
            "FC_T_label": 8,
            "FC_T": 9,
            "FC_RSSI_label": 10,
            "FC_RSSI": 11,
            "FC_SNR_label": 12,
            "FC_SNR": 13,
        }

        for name, idx in columns_map.items():
            if idx < raw.shape[1]:
                df[name] = raw[idx]
            else:
                df[name] = pd.NA

        expected_labels_ok = (
            df["GS_T_label"].eq("GS_T") &
            df["GS_RSSI_label"].eq("GS_RSSI") &
            df["GS_SNR_label"].eq("GS_SNR") &
            df["FC_T_label"].eq("FC_T") &
            df["FC_RSSI_label"].eq("FC_RSSI") &
            df["FC_SNR_label"].eq("FC_SNR")
        )

        numeric_cols = ["GS_T", "GS_RSSI", "GS_SNR", "FC_T", "FC_RSSI", "FC_SNR"]
        for col in numeric_cols:
            df[col] = pd.to_numeric(df[col], errors="coerce")

        df["valid_row"] = (~df["is_special_row"]) & expected_labels_ok & df[numeric_cols].notna().all(axis=1)
        df["bad_row"] = (~df["valid_row"]) & (~df["is_special_row"])

        return df

    def count_bad_rows(self) -> None:
        bad_rows = self.df[self.df["bad_row"]]
        print(f"bad rows: {len(bad_rows)} out of {len(self.df)}")

        if not bad_rows.empty:
            print("\nFirst bad rows:")
            cols_to_show = [
                "packet_id", "flag",
                "GS_T_label", "GS_T",
                "GS_RSSI_label", "GS_RSSI",
                "GS_SNR_label", "GS_SNR",
                "FC_T_label", "FC_T",
                "FC_RSSI_label", "FC_RSSI",
                "FC_SNR_label", "FC_SNR"
            ]
            print(bad_rows[cols_to_show].head(20).to_string(index=False))

    def count_crc_errors(self) -> None:
        crc_rows = self.df[self.df["is_crc_error"]]
        print(f"RX errors: {len(crc_rows)}")

    def count_ping_ack_rows(self) -> None:
        ping_rows = self.df[self.df["is_ping_ack"]]
        print(f"ping_ack rows ignored: {len(ping_rows)}")

    def print_averages(self) -> None:
        valid = self.df[self.df["valid_row"]].copy()

        print(f"\nvalid rows used: {len(valid)} out of {len(self.df)}")

        if valid.empty:
            print("No valid rows found. Cannot compute averages.")
            return

        print("\nAverages over valid rows only:")
        print(f"GS RSSI avg: {valid['GS_RSSI'].mean():.3f}")
        print(f"GS SNR  avg: {valid['GS_SNR'].mean():.3f}")
        print(f"FC RSSI avg: {valid['FC_RSSI'].mean():.3f}")
        print(f"FC SNR  avg: {valid['FC_SNR'].mean():.3f}")

    def print_total_test_time(self) -> None:
        valid = self.df[self.df["valid_row"]]

        if valid.empty:
            print("\nNo valid rows found. Cannot compute total test time.")
            return

        total_time = valid["GS_T"].iloc[-1]
        print(f"\nTotal test run time: {total_time:.3f} s")

    def print_telemetry_success_percentage(self) -> None:
        valid = self.df[self.df["valid_row"]].copy()

        if valid.empty:
            print("\nNo valid rows found. Cannot compute telemetry success percentage.")
            return

        valid["packet_id_num"] = pd.to_numeric(valid["packet_id"], errors="coerce")
        valid = valid.dropna(subset=["packet_id_num"]).copy()

        if valid.empty:
            print("\nNo numeric packet IDs found. Cannot compute telemetry success percentage.")
            return

        valid["packet_id_num"] = valid["packet_id_num"].astype(int)

        dropped_sequences = 0
        prev_id = valid["packet_id_num"].iloc[0]

        for curr_id in valid["packet_id_num"].iloc[1:]:
            if curr_id > prev_id + 1:
                dropped_sequences += (curr_id - prev_id - 1)
            prev_id = curr_id

        received_packets = len(valid)
        expected_packets = received_packets + dropped_sequences

        if expected_packets == 0:
            print("\nTelemetry success percentage: undefined")
            return

        success_pct = 100.0 * received_packets / expected_packets

        print(f"\nTelemetry packets received successfully: {success_pct:.3f}%")
        print(f"Received telemetry rows: {received_packets}")
        print(f"Dropped telemetry packets: {dropped_sequences}")
        print(f"Expected telemetry packets: {expected_packets}")

    def print_command_success_percentage(self) -> None:
        valid = self.df[self.df["valid_row"]].copy()

        if valid.empty:
            print("\nNo valid rows found. Cannot compute command success percentage.")
            return

        flags = valid["flag"].astype(str).str.strip()

        is_cts = flags.eq("CTS")
        total_cts = is_cts.sum()

        if total_cts == 0:
            print("\nNo CTS rows found. Cannot compute command success percentage.")
            return

        next_flag = flags.shift(-1)
        successful_cts = (is_cts & next_flag.eq("ACK")).sum()

        success_pct = 100.0 * successful_cts / total_cts

        print(f"\nCommand packet success percentage: {success_pct:.3f}%")
        print(f"CTS rows: {total_cts}")
        print(f"CTS followed by ACK: {successful_cts}")
        print(f"CTS not followed by ACK: {total_cts - successful_cts}")

if __name__ == "__main__":
    data = Data(FILE_NAME)
    print("Antenna " + FILE_NAME.split("/")[-1])
    data.count_ping_ack_rows()
    data.count_crc_errors()
    data.count_bad_rows()
    data.print_averages()
    data.print_total_test_time()
    data.print_telemetry_success_percentage()
    data.print_command_success_percentage()