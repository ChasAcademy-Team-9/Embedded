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

# ---- Struct format ----
# uint8_t SensorId
# uint32_t timestamp
# float temperature
# float humidity
# bool error (1 byte)
# uint8_t errorType
SENSORDATA_STRUCT = struct.Struct("<BIffBB")


class SensorData:
    def __init__(self, sensor_id, timestamp, temperature, humidity, error=False, errorType=0):
        self.SensorId = sensor_id
        self.timestamp = timestamp
        self.temperature = temperature
        self.humidity = humidity
        self.error = error
        self.errorType = errorType

    def to_bytes(self):
        return SENSORDATA_STRUCT.pack(
            self.SensorId,
            self.timestamp,
            self.temperature,
            self.humidity,
            int(self.error),
            self.errorType
        )


def generate_batch(sensor_id):
    now = int(time.time())  # use seconds if your C++ uses epoch seconds
    return [
        SensorData(
            sensor_id,
            now + i,
            random.uniform(15, 30),
            random.uniform(30, 70)
        )
        for i in range(BATCH_SIZE)
    ]


def send_batch(sensor_id, batch_id):
    try:
        delay = random.uniform(0, MAX_DELAY)
        time.sleep(delay)

        batch = generate_batch(sensor_id)
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
        futures = [executor.submit(send_batch, i + 1, 0) for i in range(N_SENSORS)]
        for future in as_completed(futures):
            if future.result():
                successes += 1

    elapsed = time.time() - start
    print(f"\nAll batches sent in {elapsed:.2f} seconds")
    print(f"Success rate: {successes}/{N_SENSORS} ({successes / N_SENSORS * 100:.1f}%)")


if __name__ == "__main__":
    main()
