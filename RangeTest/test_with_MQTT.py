import subprocess
import sys
import time
from datetime import datetime
import paho.mqtt.client as mqtt
import os

# ==========================================================
# HARD-CODE HERE
# ==========================================================
PROJECT_DIR = r"C:\Jeff_Documents\Programming\MRT\radios-2026\RangeTest"
UPLOAD_PORT = "COM4"

# MQTT Configuration
MQTT_BROKER_IP = "127.0.0.1"  # localhost - MQTT broker runs on this computer
MQTT_BROKER_PORT = 1883
MQTT_LOGS_TOPIC = "groundstation/logs"

# =========================================================

PIO_ENV = "teensy41"

FILE_NAME = "test" + datetime.now().strftime("%d_%H_%M")
LOG_FILE = r"C:\Jeff_Documents\Programming\MRT\radios-2026\RangeTest\logs\\" + FILE_NAME + ".txt"

# Ensure logs directory exists
os.makedirs(os.path.dirname(LOG_FILE), exist_ok=True)

# MQTT broker process
mqtt_process = None

# ==========================================================


def start_mqtt_broker() -> subprocess.Popen:
    """Start the MQTT broker (mosquitto)"""
    print("[INFO] Starting MQTT broker...")
    try:
        proc = subprocess.Popen(
            ["mosquitto", "-p", str(MQTT_BROKER_PORT)],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        time.sleep(2)  # Give broker time to start
        print("[INFO] MQTT broker started successfully")
        return proc
    except FileNotFoundError:
        print("[ERROR] mosquitto not found. Please install it:")
        print("       Windows: choco install mosquitto")
        print("       macOS: brew install mosquitto")
        print("       Linux: sudo apt-get install mosquitto")
        return None


def stop_mqtt_broker(proc: subprocess.Popen):
    """Stop the MQTT broker"""
    if proc:
        print("[INFO] Stopping MQTT broker...")
        proc.terminate()
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()
        print("[INFO] MQTT broker stopped")


def pio_upload() -> int:
    cmd = [
        "pio",
        "run",
        "-t",
        "upload",
        "-e",
        PIO_ENV,
        "--upload-port",
        UPLOAD_PORT,
    ]

    print("[INFO] Uploading via PlatformIO...")
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


def on_message(client, userdata, msg):
    """Callback when MQTT message is received"""
    text = msg.payload.decode(errors="replace").strip()
    with open(LOG_FILE, "a", buffering=1) as f:
        f.write(text + "\n")
    print("[LOG] " + text)


def on_connect(client, userdata, flags, rc):
    """Callback when client connects to broker"""
    if rc == 0:
        print(f"[INFO] Connected to MQTT broker at {MQTT_BROKER_IP}:{MQTT_BROKER_PORT}")
        client.subscribe(MQTT_LOGS_TOPIC)
        print(f"[INFO] Subscribed to {MQTT_LOGS_TOPIC}")
    else:
        print(f"[ERROR] Failed to connect to MQTT broker (code {rc})")


def on_disconnect(client, userdata, rc):
    """Callback when client disconnects"""
    if rc != 0:
        print(f"[ERROR] Unexpected disconnection (code {rc})")
    else:
        print("[INFO] Disconnected from MQTT broker")


def log_mqtt():
    """Connect to MQTT broker and log messages"""
    print(f"[INFO] Waiting for Teensy to boot and connect to network...")
    time.sleep(5)

    print(f"[INFO] Connecting to MQTT broker at {MQTT_BROKER_IP}:{MQTT_BROKER_PORT}...")
    print(f"[INFO] Logging to {LOG_FILE}")
    print("       Ctrl+C to stop")

    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_message = on_message

    try:
        client.connect(MQTT_BROKER_IP, MQTT_BROKER_PORT, keepalive=60)

        with open(LOG_FILE, "a", buffering=1) as f:
            f.write(f"\n=== Session start {datetime.now().isoformat()} ===\n")

        # Blocking call that processes network traffic, dispatches callbacks and handles reconnecting
        client.loop_forever()

    except KeyboardInterrupt:
        print("\n[INFO] Stopping logging...")
        with open(LOG_FILE, "a", buffering=1) as f:
            f.write(f"=== Session end {datetime.now().isoformat()} ===\n")
        client.disconnect()

    except ConnectionRefusedError:
        print(
            f"[ERROR] Could not connect to MQTT broker at {MQTT_BROKER_IP}:{MQTT_BROKER_PORT}"
        )
        print("[ERROR] Make sure mosquitto is running and accessible")
        return
    except Exception as e:
        print(f"[ERROR] {e}")
        return


def main() -> int:
    global mqtt_process

    # Start MQTT broker
    mqtt_process = start_mqtt_broker()
    if mqtt_process is None:
        return 1

    try:
        # Upload firmware
        rc = pio_upload()
        if rc != 0:
            print(f"[ERROR] Upload failed (exit code {rc}).")
            return rc

        # Log via MQTT
        log_mqtt()
        return 0

    finally:
        # Cleanup
        stop_mqtt_broker(mqtt_process)


if __name__ == "__main__":
    raise SystemExit(main())
