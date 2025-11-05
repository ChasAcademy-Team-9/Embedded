# Chas Advance ESP32 Project

## Projektöversikt
ESP32-baserat sensor system för temperatur- och luftfuktighetsmätning med WiFi-anslutning och datahantering.

## Hårdvara
- **Plattform**: Adafruit Feather ESP32-S3
- **Sensorer**: Temperatur och luftfuktighet
- **Anslutning**: WiFi för dataöverföring

## Projektstruktur

```
Chas Advance ESP32/
├── src/                    # Källkod
│   ├── main.cpp           # Huvudprogram
│   ├── wifiHandler.cpp    # WiFi hantering
│   ├── sensorDataHandler.cpp # Sensor data processing
│   ├── jsonParser.cpp     # JSON parsing och serialisering
│   ├── espLogger.cpp      # Logging funktionalitet
│   └── mockJson.cpp       # Mock data för utveckling
├── include/               # Header filer
│   ├── wifiHandler.h
│   ├── sensorDataHandler.h
│   ├── jsonParser.h
│   ├── espLogger.h
│   ├── mockJson.h
│   ├── log.h
│   └── ESPSECRETS.h      # WiFi credentials (gitignored)
├── lib/                   # Bibliotek
│   └── ArduinoMock/      # Arduino mock för native testing
├── test/                  # Enhetstester
│   ├── test_*.cpp        # Test implementationer
│   └── README.md         # Test dokumentation
├── platformio.ini         # PlatformIO konfiguration
└── README.md             # Denna fil
```

## Funktionalitet

### Huvudfunktioner
- **Sensor Data Collection**: Läser temperatur och luftfuktighet
- **Data Processing**: Median beräkning och felhantering
- **JSON Serialization**: Formaterar data för överföring
- **WiFi Communication**: Skickar data till server
- **Error Handling**: Robust felhantering för sensor errors
- **Logging**: Omfattande logging av events och data

### Dataflöde
1. Sensor data läses från hårdvara
2. Data processas med median filtrering
3. Data serialiseras till JSON format
4. JSON skickas via WiFi till server
5. Events och errors loggas för debugging

## Utveckling

### Bygga och Ladda upp
```bash
# Bygga projektet
platformio run

# Ladda upp till ESP32
platformio run --target upload

# Serial monitor
platformio device monitor
```

### Environments
- **esp32s3**: Produktion environment för ESP32-S3 hårdvara
- **native**: Test environment för native unit testing

## Testing

Detta projekt har omfattande enhetstester som körs i native environment utan hårdvaruberoende.

### Kör alla tester
```bash
platformio test -e native
```

### Test Coverage
- **22 enhetstester** täcker alla huvudfunktioner
- **JSON parsing och serialisering**
- **Sensor data processing**
- **Error handling scenarios**
- **Mock funktionalitet**

Se [`test/README.md`](test/README.md) för detaljerad test dokumentation.

## Konfiguration

### WiFi Setup
1. Skapa `include/ESPSECRETS.h` baserat på template
2. Fyll i dina WiFi credentials:
```cpp
#define WIFI_SSID "ditt_wifi_namn"
#define WIFI_PASS "ditt_wifi_lösenord"
```

### Dependencies
Alla dependencies hanteras automatiskt av PlatformIO:
- **ArduinoJson**: JSON parsing och serialisering
- **WiFi**: ESP32 WiFi bibliotek
- **Unity**: Test framework (endast för testing)

## API och Dataformat

### JSON Data Format
```json
{
  "ArduinoID": 1,
  "SensorTimeStamp": "2024-01-01 12:00:00",
  "Temperature": 22.5,
  "Humidity": 45.0
}
```

### Batch Format
```json
[
  {
    "ArduinoID": 1,
    "SensorTimeStamp": "2024-01-01 12:00:00",
    "Temperature": 22.5,
    "Humidity": 45.0
  }
]
```

## Logging

### Log Nivåer
- **Sensor Data**: Loggning av alla sensor readings
- **Events**: System events och state changes
- **Errors**: Felhantering och debugging information

### Error Types
```cpp
enum ErrorType {
    NO_ERROR = 0,
    SENSOR_ERROR = 1,
    WIFI_ERROR = 2,
    JSON_ERROR = 3
};
```

## Felsökning

### Vanliga Problem
1. **WiFi Anslutning**: Kontrollera credentials i ESPSECRETS.h
2. **Sensor Readings**: Verifiera hårdvaruanslutningar
3. **JSON Parsing**: Använd serial output för debugging
4. **Memory Issues**: Övervaka heap usage

### Debug Tips
- Använd `Serial.println()` för debugging output
- Kontrollera error logs för specifika felkoder
- Testa med mock data först innan hårdvarutestning

## Bidra till Projektet

### Code Style
- Följ PlatformIO/Arduino conventions
- Kommentera komplexa funktioner
- Använd beskrivande variabelnamn

### Testing
- Skriv tester för nya funktioner
- Kör alla tester innan commit
- Uppdatera test dokumentation

### Git Workflow
1. Skapa feature branch från main
2. Implementera ändringar med tester
3. Kör alla tester och verifiera att de passerar
4. Skapa Pull Request med detaljerad beskrivning

## License
Detta projekt är utvecklat för Chas Academy Team 9.