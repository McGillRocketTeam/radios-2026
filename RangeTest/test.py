import subprocess
import sys
import time
from datetime import datetime

import serial

# ==========================================================
# HARD-CODE HERE
# ==========================================================
PROJECT_DIR = r"C:\Jeff_Documents\Programming\MRT\radios-2026\RangeTest"
UPLOAD_PORT = "COM6"

# =========================================================

BAUD        = 115200
WAIT_SECS   = 5
PIO_ENV     = "teensy41"

LOG_FILE    = r"C:\Jeff_Documents\Programming\MRT\radios-2026\RangeTest\logs"
# ==========================================================


def pio_upload() -> int:
    cmd = [
        "pio", "run",
        "-t", "upload",
        "-e", PIO_ENV,
        "--upload-port", UPLOAD_PORT,
    ]

    print(f"[INFO] Uploading via PlatformIO...")
    print("       " + " ".join(cmd))

    try:
        proc = subprocess.run(
            cmd,
            cwd=PROJECT_DIR,
            stdout=sys.stdout,
            stderr=sys.stderr,
        )
        return proc.returncode
    except FileNotFoundError:
        print("[ERROR] PlatformIO ('pio') not found on PATH.")
        return 127


def log_serial():
    print(f"[INFO] Waiting {WAIT_SECS}s for Teensy reboot...")
    time.sleep(WAIT_SECS)

    print(f"[INFO] Opening serial port {UPLOAD_PORT} @ {BAUD}")
    print(f"[INFO] Logging to {LOG_FILE}")
    print("       Ctrl+C to stop")

    with serial.Serial(UPLOAD_PORT, BAUD, timeout=0.5) as ser, \
         open(LOG_FILE, "a", buffering=1) as f:

        f.write(f"\n=== Session start {datetime.now().isoformat()} ===\n")

        try:
            while True:
                line = ser.readline()
                if not line:
                    continue

                text = line.decode(errors="replace")
                f.write(text)

        except KeyboardInterrupt:
            print("\n[INFO] Logging stopped.")
            f.write(f"=== Session end {datetime.now().isoformat()} ===\n")


def main() -> int:
    rc = pio_upload()
    if rc != 0:
        print(f"[ERROR] Upload failed (exit code {rc}).")
        return rc

    log_serial()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
