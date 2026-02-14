import subprocess
import sys
import time
from datetime import datetime
import serial
import serial.tools.list_ports

# ==========================================================
# HARD-CODE HERE
# ==========================================================
PROJECT_DIR = r"C:\Jeff_Documents\Programming\MRT\radios-2026\GroundStation"
UPLOAD_PORT = "COM4"

# =========================================================

BAUD        = 115200
WAIT_SECS   = 2
PIO_ENV     = "teensy41" 

FILE_NAME = "test" + datetime.now().strftime("%d_%H_%M")
LOG_FILE    = r"C:\Jeff_Documents\Programming\MRT\radios-2026\GroundStation\logs\\" + FILE_NAME + ".txt"
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



def wait_for_port(port: str, timeout=15.0):
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

    print(f"[INFO] Opening serial port {UPLOAD_PORT} @ {BAUD}")
    print(f"[INFO] Logging to {LOG_FILE}")
    print("       Ctrl+C to stop")

    with serial.Serial(UPLOAD_PORT, BAUD, timeout=0.5) as ser, \
         open(LOG_FILE, "a", buffering=1) as f:

        time.sleep(0.2)
        ser.write(b"ping\n")
        ser.flush()

        f.write(f"\n=== Session start {datetime.now().isoformat()} ===\n")

        try:
            while True:
                line = ser.readline()
                if not line:
                    continue
                text = line.decode(errors="replace").rstrip()
                f.write(text + "\n")
                print("[LOG] " + text)

        except KeyboardInterrupt:
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
