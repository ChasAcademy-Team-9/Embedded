import socket
import struct
import random
import time
from concurrent.futures import ThreadPoolExecutor, as_completed

ESP_IP = "192.168.0.104"  # ESP32 STA IP
ESP_PORT = 80
NUM_SENSORS = 10
BATCH_SIZE = 5  # number of SensorData per batch
TIMEOUT = 3     # connect timeout in seconds

# Simulate one SensorData
class SensorData:
    def __init__(self):
        self.timestamp = random.randint(0, 100000)  # Arduino millis
        self.temperature = round(random.uniform(20, 30), 2)
        self.humidity = round(random.uniform(30, 70), 2)
        self.error = 0  # false
        self.errorType = 0

    def to_bytes(self):
        # Struct layout: uint32_t, float, float, bool, uint8_t, padding 3 bytes
        return struct.pack('<IffBxxx', self.timestamp, self.temperature, self.humidity, self.errorType)

def send_batch(sensor_id, batch_id):
    sendMillis = int(time.time() * 1000) & 0xFFFFFFFF  # simulate Arduino millis
    batch = [SensorData() for _ in range(BATCH_SIZE)]

    payload = struct.pack('<I', sendMillis) + b''.join([s.to_bytes() for s in batch])
    content_length = len(payload)

    # Build raw HTTP request
    req = (
        f"POST /data HTTP/1.1\r\n"
        f"Host: {ESP_IP}\r\n"
        f"Content-Type: application/octet-stream\r\n"
        f"Content-Length: {content_length}\r\n"
        f"Connection: close\r\n\r\n"
    ).encode('utf-8') + payload

    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(TIMEOUT)
        s.connect((ESP_IP, ESP_PORT))
        s.sendall(req)
        resp = s.recv(1024)
        s.close()
        if b"200" in resp:
            print(f"Sensor {sensor_id}, batch {batch_id}: success")
            return True
        else:
            print(f"Sensor {sensor_id}, batch {batch_id}: failed, response: {resp}")
            return False
    except Exception as e:
        print(f"Sensor {sensor_id}, batch {batch_id}: failed -> {e}")
        return False

def main():
    results = []
    with ThreadPoolExecutor(max_workers=20) as executor:
        futures = []
        for sensor_id in range(NUM_SENSORS):
            # Each sensor sends one batch
            futures.append(executor.submit(send_batch, sensor_id, 0))
        for future in as_completed(futures):
            results.append(future.result())

    success_count = sum(results)
    print(f"\nAll batches sent: {success_count}/{NUM_SENSORS} successful")

if __name__ == "__main__":
    main()
