import subprocess
import sys
import time
from datetime import datetime
from pathlib import Path

import serial
import serial.tools.list_ports

# ==========================================================
# HARD-CODE HERE
# ==========================================================
UPLOAD_PORT = "COM3"

# =========================================================

PROJECT_DIR = Path.cwd()

BAUD = 115200
WAIT_SECS = 2
PIO_ENV = "RANGETEST_GS"

FILE_NAME = "test_" + datetime.now().strftime("%d_%H_%M")
LOGS_DIR = PROJECT_DIR / "logs"
LOG_FILE = LOGS_DIR / f"{FILE_NAME}.csv"

# ==========================================================


def pio_upload() -> int:
    cmd = [
        "pio", "run",
        "-t", "upload",
        "-e", PIO_ENV,
        "--upload-port", UPLOAD_PORT,
    ]

    print("[INFO] Uploading via PlatformIO...")
    print("       " + " ".join(cmd))

    try:
        proc = subprocess.run(
            cmd,
            cwd=str(PROJECT_DIR),
            stdout=sys.stdout,
            stderr=sys.stderr,
        )
        return proc.returncode
    except FileNotFoundError:
        print("[ERROR] PlatformIO ('pio') not found on PATH.")
        return 127


def wait_for_port(port: str, timeout: float = 15.0) -> bool:
    start = time.time()
    while time.time() - start < timeout:
        ports = [p.device for p in serial.tools.list_ports.comports()]
        if port in ports:
            return True
        time.sleep(0.25)
    return False


def log_serial():
    print(f"[INFO] Waiting for serial port {UPLOAD_PORT} to appear...")

    if not wait_for_port(UPLOAD_PORT):
        print(f"[ERROR] {UPLOAD_PORT} did not appear.")
        return

    LOGS_DIR.mkdir(parents=True, exist_ok=True)

    print(f"[INFO] Opening serial port {UPLOAD_PORT} @ {BAUD}")
    print(f"[INFO] Logging to {LOG_FILE}")
    print("       Ctrl+C to stop")

    with serial.Serial(UPLOAD_PORT, BAUD, timeout=0.5) as ser, \
         open(LOG_FILE, "a", buffering=1, encoding="utf-8") as f:

        time.sleep(2)
        ser.write(b"ping\n")
        ser.flush()

        try:
            while True:
                line = ser.readline()
                if not line:
                    continue

                text = line.decode(errors="replace").rstrip()
                f.write(text + "\n")
                print("[LOG] " + text)

        except KeyboardInterrupt:
            print("\n[INFO] Logging stopped.")


def main() -> int:
    rc = pio_upload()
    if rc != 0:
        print(f"[ERROR] Upload failed (exit code {rc}).")
        return rc

    time.sleep(WAIT_SECS)
    log_serial()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())