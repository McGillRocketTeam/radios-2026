#!/usr/bin/env python3
"""Upload RANGETEST_GS and collect its serial output on macOS.

Run these commands from the GroundStation directory:
  Install dependencies: python3 -m pip install pyserial platformio
  List serial ports:    python3 rangesTestCollectMac.py --list-ports
  Run (auto-detect GS): python3 rangesTestCollectMac.py
  Run (choose GS port): python3 rangesTestCollectMac.py --port /dev/cu.usbmodem123

Close other serial monitors before running. Press Ctrl+C to stop logging.
"""

import argparse
import shutil
import subprocess
import sys
import time
from datetime import datetime
from pathlib import Path

import serial
import serial.tools.list_ports

PROJECT_DIR = Path(__file__).resolve().parent
BAUD = 115200
PIO_ENV = "RANGETEST_GS"


def available_ports():
    # macOS callout devices avoid the carrier-detect behavior of /dev/tty.*.
    return sorted(
        (p for p in serial.tools.list_ports.comports() if p.device.startswith("/dev/cu.")),
        key=lambda p: p.device,
    )


def select_port(requested, ports):
    if requested:
        for port in ports:
            if port.device == requested:
                return port
        raise ValueError(f"Serial port {requested} was not found. Use --list-ports.")

    candidates = [p for p in ports if p.vid == 0x16C0]
    if not candidates:
        candidates = [p for p in ports if p.device.startswith(("/dev/cu.usbmodem", "/dev/cu.usbserial"))]
    if len(candidates) == 1:
        return candidates[0]
    if not candidates:
        raise ValueError("No USB serial device found. Connect the GS and use --list-ports.")
    names = ", ".join(p.device for p in candidates)
    raise ValueError(f"Multiple USB serial devices found: {names}. Select the GS with --port.")


def pio_command():
    for name in ("pio", "platformio"):
        executable = shutil.which(name)
        if executable:
            return executable
    # PlatformIO installed by the VS Code extension may not be on the shell PATH.
    for name in ("pio", "platformio"):
        executable = Path.home() / ".platformio" / "penv" / "bin" / name
        if executable.is_file():
            return str(executable)
    raise ValueError("PlatformIO not found. Install it with: python3 -m pip install platformio")


def wait_for_port(original, timeout=15.0):
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        ports = available_ports()
        # USB enumeration can change the device path after flashing.
        if original.serial_number:
            matches = [p for p in ports if p.serial_number == original.serial_number and p.vid == original.vid]
            if len(matches) == 1:
                return matches[0].device
        for port in ports:
            if port.device == original.device:
                return port.device
        time.sleep(0.25)
    raise ValueError(f"{original.device} did not reappear after upload.")


def log_serial(port):
    logs_dir = PROJECT_DIR / "logs"
    logs_dir.mkdir(parents=True, exist_ok=True)
    log_file = logs_dir / f"test_{datetime.now():%Y_%m_%d_%H_%M_%S_%f}.csv"
    print(f"[INFO] Opening {port} @ {BAUD}")
    print(f"[INFO] Logging to {log_file}\n       Ctrl+C to stop")

    with serial.Serial(port, BAUD, timeout=0.5) as ser, \
            log_file.open("x", buffering=1, encoding="utf-8") as output:
        time.sleep(2)
        ser.write(b"ping\n")
        ser.flush()
        while True:
            line = ser.readline()
            if line:
                text = line.decode(errors="replace").rstrip()
                output.write(text + "\n")
                print("[LOG] " + text)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", help="GS serial port, e.g. /dev/cu.usbmodem123")
    parser.add_argument("--list-ports", action="store_true", help="List macOS serial ports and exit")
    args = parser.parse_args()

    try:
        ports = available_ports()
        if args.list_ports:
            for port in ports:
                print(f"{port.device}: {port.description}")
            if not ports:
                print("No /dev/cu.* serial ports found.")
            return 0

        original = select_port(args.port, ports)
        command = [pio_command(), "run", "-t", "upload", "-e", PIO_ENV,
                   "--upload-port", original.device]
        print(f"[INFO] Uploading {PIO_ENV} to {original.device}...", flush=True)
        result = subprocess.run(command, cwd=PROJECT_DIR)
        if result.returncode:
            print(f"[ERROR] Upload failed (exit code {result.returncode}).")
            return result.returncode

        time.sleep(2)
        log_serial(wait_for_port(original))
    except KeyboardInterrupt:
        print("\n[INFO] Logging stopped.")
    except (ValueError, OSError, serial.SerialException) as error:
        print(f"[ERROR] {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
