import requests
import struct
import random
import time
import threading
import sys

# ---- Configuration ----
ESP_HOST = "192.168.0.104"   # ESP32 STA IP
ESP_PORT = 80

# Define the exact Sensor IDs to simulate (use integers 0-255)
sensorIDs = [1, 2, 3, 10, 42]   # <-- edit this list as needed

BATCH_SIZE = 10            # entries per batch
SEND_INTERVAL = 30        # seconds between batches per sensor
MAX_JITTER = 2.0          # random jitter (seconds) added to each send time
CONNECT_TIMEOUT = 3
READ_TIMEOUT = 5

# ---- Struct format (must match your C++ SensorData layout) ----
# uint8_t SensorId
# uint32_t timestamp
# float temperature
# float humidity
# uint8_t error (0/1)
# uint8_t errorType
SENSORDATA_STRUCT = struct.Struct("<BIffBB")

HEADERS = {
    "Content-Type": "application/octet-stream",
    "Connection": "close"
}

def now_millis_32():
    return int(time.time() * 1000) & 0xFFFFFFFF

class SensorData:
    def __init__(self, sensor_id, timestamp, temperature, humidity, error=0, errorType=0):
        self.SensorId = sensor_id
        self.timestamp = timestamp
        self.temperature = temperature
        self.humidity = humidity
        self.error = int(error)
        self.errorType = errorType

    def to_bytes(self):
        return SENSORDATA_STRUCT.pack(
            self.SensorId,
            int(self.timestamp),
            float(self.temperature),
            float(self.humidity),
            self.error,
            self.errorType
        )

def generate_batch(sensor_id):
    base_ts = int(time.time())  # seconds since epoch
    batch = []
    for i in range(BATCH_SIZE):
        batch.append(SensorData(
            sensor_id,
            base_ts + i,
            random.uniform(15.0, 30.0),
            random.uniform(30.0, 70.0),
            error=0,
            errorType=0
        ))
    return batch

def send_batch_once(sensor_id):
    try:
        batch = generate_batch(sensor_id)
        sendMillis = now_millis_32()
        payload = struct.pack("<I", sendMillis) + b"".join(e.to_bytes() for e in batch)

        resp = requests.post(
            f"http://{ESP_HOST}:{ESP_PORT}/data",
            headers=HEADERS,
            data=payload,
            timeout=(CONNECT_TIMEOUT, READ_TIMEOUT)
        )
        ok = resp.status_code == 200
        print(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] Sensor {sensor_id:03} -> {resp.status_code} {'OK' if ok else 'FAIL'}")
        return ok
    except Exception as e:
        print(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] Sensor {sensor_id:03} -> EXC {e}")
        return False

def sensor_loop(sensor_id, interval, stop_event):
    # Stagger first send with a small random offset to avoid synchronized spikes
    initial_delay = random.uniform(0, min(MAX_JITTER, interval))
    time.sleep(initial_delay)
    while not stop_event.is_set():
        # optional small jitter per send
        jitter = random.uniform(-MAX_JITTER, MAX_JITTER)
        if jitter > 0:
            time.sleep(jitter)
        send_batch_once(sensor_id)
        # wait for interval, checking stop event
        for _ in range(int(interval)):
            if stop_event.is_set():
                break
            time.sleep(1)
        # finer-grained remainder
        rem = interval - int(interval)
        if rem > 0 and not stop_event.is_set():
            time.sleep(rem)

def main():
    threads = []
    stop_event = threading.Event()

    try:
        print(f"Starting {len(sensorIDs)} sensor threads, each sending {BATCH_SIZE} entries every {SEND_INTERVAL}s")
        for sid in sensorIDs:
            t = threading.Thread(target=sensor_loop, args=(sid, SEND_INTERVAL, stop_event), daemon=True)
            t.start()
            threads.append(t)

        # Run until interrupted
        while True:
            time.sleep(1)

    except KeyboardInterrupt:
        print("\nStopping simulation...")
        stop_event.set()
        # give threads a moment to exit
        for t in threads:
            t.join(timeout=2)
        print("Stopped.")
        sys.exit(0)

if __name__ == "__main__":
    main()