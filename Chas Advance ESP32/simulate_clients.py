import requests
import struct
import random
import time
from concurrent.futures import ThreadPoolExecutor, as_completed

ESP_HOST = "192.168.0.104"   # <-- your ESP32 STA IP
ESP_PORT = 80
N_SENSORS = 100              # total simulated sensors
BATCH_SIZE = 10              # entries per batch
MAX_DELAY = 2.0              # max random delay before sending (seconds)
CONNECT_TIMEOUT = 2          # shorter connect timeout
READ_TIMEOUT = 3             # read timeout

# Simulated sensor data structure
class SensorData:
    def __init__(self, timestamp, temperature, humidity, errorType=0):
        self.timestamp = timestamp
        self.temperature = temperature
        self.humidity = humidity
        self.errorType = errorType

    def to_bytes(self):
        # Same as Arduino struct:
        # uint32_t timestamp, float temperature, float humidity, bool error, uint8_t errorType
        # We'll pack bool as a single byte (0/1)
        error_flag = 0
        return struct.pack("<IffBBxx", self.timestamp, self.temperature, self.humidity, error_flag, self.errorType)

def generate_batch():
    now = int(time.time() * 1000) & 0xFFFFFFFF  # mimic millis() wraparound
    return [SensorData(now + i * 100, random.uniform(15, 30), random.uniform(30, 70)) for i in range(BATCH_SIZE)]

def send_batch(sensor_id, batch_id):
    try:
        # Random delay before sending (simulate jitter)
        delay = random.uniform(0, MAX_DELAY)
        time.sleep(delay)

        batch = generate_batch()
        sendMillis = int(time.time() * 1000) & 0xFFFFFFFF
        payload = struct.pack("<I", sendMillis) + b"".join(s.to_bytes() for s in batch)

        headers = {
            "Content-Type": "application/octet-stream",
            "Connection": "close"
        }

        response = requests.post(
            f"http://{ESP_HOST}:{ESP_PORT}/data",
            headers=headers,
            data=payload,
            timeout=(CONNECT_TIMEOUT, READ_TIMEOUT)
        )

        success = response.status_code == 200
        print(f"Sensor {sensor_id:02}, batch {batch_id}: {'OK' if success else 'FAIL'} ({response.status_code})")
        return success

    except Exception as e:
        print(f"Sensor {sensor_id:02}, batch {batch_id}: failed -> {e}")
        return False


def main():
    start = time.time()
    successes = 0

    with ThreadPoolExecutor(max_workers=50) as executor:
        futures = [executor.submit(send_batch, i, 0) for i in range(N_SENSORS)]
        for future in as_completed(futures):
            if future.result():
                successes += 1

    elapsed = time.time() - start
    print(f"\nAll batches sent in {elapsed:.2f} seconds")
    print(f"Success rate: {successes}/{N_SENSORS} ({successes / N_SENSORS * 100:.1f}%)")


if __name__ == "__main__":
    main()
