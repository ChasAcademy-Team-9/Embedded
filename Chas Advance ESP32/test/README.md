# ESP32 Unit Tests

Detta projekt använder PlatformIO Unity Test Framework för enhetstestning av ESP32 kod i native environment utan hårdvaruberoende.

## Översikt

Testerna körs i native environment med ett komplett Arduino mock-bibliotek som emulerar Arduino API:t. Detta möjliggör snabb och pålitlig testning utan behov av fysisk ESP32 hårdvara.

## Test Coverage

### Nuvarande test-struktur (22 tester totalt):

#### JSON Generation Tests (`test_mock_json.cpp`) - 3 tester
- ✅ `test_mock_json_format` - Verifierar JSON format och struktur
- ✅ `test_mock_json_temperature_range` - Testar temperaturvärden inom förväntade intervall
- ✅ `test_mock_json_error_values` - Validerar felhantering för sensor errors

#### Sensor Data Handler Tests (`test_sensor_data_handler.cpp`) - 4 tester
- ✅ `test_median_odd_values` - Median beräkning för udda antal värden
- ✅ `test_median_even_values` - Median beräkning för jämnt antal värden
- ✅ `test_median_empty_vector` - Hantering av tom vektor
- ✅ `test_calc_median_excludes_errors` - Exkludering av error-värden (-99.0, -1.0)

#### JSON Parser Tests (`test_json_parser.cpp`) - 6 tester
- ✅ `test_serialize_batch_to_json` - Serialisering av sensor data till JSON
- ✅ `test_serialize_empty_batch` - Hantering av tom batch
- ✅ `test_parse_json_valid_data` - Parsing av giltigt JSON
- ✅ `test_parse_json_invalid_data` - Felhantering för ogiltigt JSON
- ✅ `test_parse_json_missing_fields` - Hantering av saknade JSON-fält
- ✅ `test_parse_json_array_functionality` - Testing av JSON array parsing

#### Log Function Tests (`test_log_functions.cpp`) - 3 tester
- ✅ `test_timestamp_string_to_unix` - Konvertering av timestamp strings till Unix time
- ✅ `test_format_unix_time` - Formatering av Unix time till readable format
- ✅ `test_error_type_enum` - Validering av ErrorType enum värden

#### Error Handling Tests (`test_error_handling.cpp`) - 6 tester
- ✅ `test_error_handling_null_json` - Hantering av empty/null JSON strings
- ✅ `test_error_handling_malformed_json` - Malformed JSON syntax errors
- ✅ `test_error_handling_wrong_data_types` - Fel datatyper i JSON
- ✅ `test_error_handling_empty_sensor_batch` - Tom sensor batch hantering
- ✅ `test_error_handling_median_with_all_errors` - Median med bara error-värden
- ✅ `test_error_handling_median_empty_vector` - Median med tom vektor

## Arduino Mock Library

### Lokalisering
`lib/ArduinoMock/Arduino.h` - Komplett Arduino API mock för native testing

### Funktionalitet
- **String Class**: Fullständig implementation med std::string backend
- **Serial Mock**: Testbar Serial interface med `lastPrint` och `lastPrintln`
- **Basic Arduino Types**: `byte`, `boolean`, och andra grundtyper
- **Mock Functions**: 
  - `generateMockJson()` - Returnerar test JSON data
  - `median()` - Median beräkning med error-värde filtrering
  - `serializeBatchToJson()` - JSON serialisering för sensor batches
  - `parseJson()` / `parseJsonArray()` - JSON parsing funktioner
  - `timestampStringToUnix()` / `formatUnixTime()` - Timestamp konvertering
  - `delay()` - Mock delay funktion

### String Class Features
```cpp
// Konstruktorer för olika datatyper
String(const char* str)
String(int value)
String(float value, int digits)

// Sök funktioner
int indexOf(const String& str)
int indexOf(const char* str)

// Operatorer
operator+(const String& other)
operator==(const String& other)
```

## Körning av Tester

### Kommando
```bash
cd "Chas Advance ESP32"
platformio test -e native
```

### Förväntad Output
```
========================================= 22 test cases: 22 succeeded in 00:00:10.017 =========================================
```

### Test Environment Konfiguration
I `platformio.ini`:
```ini
[env:native]
platform = native
lib_deps = 
    throwtheswitch/Unity
    bblanchon/ArduinoJson
build_flags = 
    -DNATIVE_BUILD
    -DUNIT_TEST
    -Ilib/ArduinoMock
test_framework = unity
```

## Utveckling och Underhåll

### Lägga till nya tester
1. Skapa ny test-fil i `test/` mappen med prefix `test_`
2. Inkludera `<unity.h>` och relevant headers
3. Implementera test-funktioner med `void test_function_name(void)`
4. Lägg till externa deklarationer i `test_main.cpp`
5. Lägg till `RUN_TEST(test_function_name)` i `main()` funktionen

### Mock Function Development
När nya funktioner läggs till i source koden:
1. Lägg till mock implementation i `lib/ArduinoMock/Arduino.h`
2. Använd `#ifdef NATIVE_BUILD` för mock-specifik kod
3. Se till att mock funktioner returnerar realistiska test-värden
4. Implementera felhantering för edge cases

### Best Practices
- **Test Isolation**: Varje test ska vara oberoende
- **Clear Assertions**: Använd specifika Unity assertions
- **Mock Behavior**: Mock funktioner ska simulera verkligt beteende
- **Error Testing**: Testa både framgångsfall och felscenarier
- **Serial Output**: Rensa `Serial.lastPrint` innan tester som förväntar output

## Dependencies

- **Unity Test Framework**: Huvudsakligt test framework
- **ArduinoJson**: JSON parsing och serialisering
- **Custom Arduino Mock**: Lokal implementation för Arduino API emulering

## Troubleshooting

### Vanliga Problem

#### Compilation Errors
- Kontrollera att alla mock funktioner är deklarerade korrekt
- Verifiera att `#ifdef NATIVE_BUILD` används för mock-specifik kod

#### Test Failures
- Kontrollera mock function behavior matchar förväntningar
- Verifiera att Serial mock rensas mellan tester
- Säkerställ att test data är konsistent

#### Linking Errors
- Kontrollera att alla externa funktioner har mock implementations
- Verifiera att `SerialMock Serial;` endast definieras en gång (i test_main.cpp)

### Debug Tips
- Använd `-vvv` flagga för verbose output: `platformio test -e native -vvv`
- Lägg till debug prints i mock funktioner för att spåra execution flow
- Verifiera att test environment flags är korrekt satta

## Framtida Förbättringar

- **WiFi Mock Functions**: Mock för WiFi connectivity testing
- **EEPROM Mock**: Simulering av EEPROM operations
- **Timer Mock**: Mock för tidsberoende funktioner
- **Integration Tests**: Tester som kombinerar flera moduler
- **Performance Tests**: Benchmarking av kritiska funktioner